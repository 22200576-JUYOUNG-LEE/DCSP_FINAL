function Linear_mapping(filename)
% Data loading
data = readmatrix(filename, 'NumHeaderLines', 1, "FileType", "text");
V_c = data(:, 1); 
w_gyro = data(:, 2); 


[coeff_pos, coeff_neg, pos_start, neg_start] = Get_fitting_Quadratic(filename);


a = coeff_pos(1);   b = coeff_pos(2);   c = coeff_pos(3);   
a1 = coeff_neg(1);  b1 = coeff_neg(2);  c1 = coeff_neg(3);   

% automatically finding positive & negative points of starting. 
pos_indices = V_c > pos_start;
real_pos_start = min(V_c(pos_indices)); 


mod_real_pos_start=2.76;
mod_real_neg_start=2.24;

deadzone_pos_start=2.74;
deadzone_nge_start=2.26;

neg_indices = V_c < neg_start;
real_neg_start = max(V_c(neg_indices)); 


w_pos_start = a * (mod_real_pos_start)^2 + b * mod_real_pos_start + c;
w_neg_start = a1 * (mod_real_neg_start)^2 + b1 * mod_real_neg_start + c1;


% Three point mapping
V_dz = [mod_real_neg_start; 2.5; mod_real_pos_start];
W_dz = [-55.6;    0;   56.8];

A_dz = [V_dz.^2, V_dz, ones(3,1)];
coeff_dz = A_dz \ W_dz;  % [a_dz, b_dz, c_dz]

a_dz = coeff_dz(1);
b_dz = coeff_dz(2);
c_dz = coeff_dz(3);
% ====================================================================

data_matrix = [a_dz, b_dz, c_dz];

writematrix(data_matrix, '..\data\LIN_DZ_COEF.csv');


w_model = zeros(size(V_c)); 
for i = 1:length(V_c)
    if V_c(i) >= mod_real_pos_start 
        
        w_model(i) = a*(V_c(i))^2 + b*V_c(i) + c;
    elseif V_c(i) <= mod_real_neg_start 
       
        w_model(i) = a1*(V_c(i))^2 + b1*V_c(i) + c1;

    elseif V_c(i) <= deadzone_pos_start && V_c(i) >= 2.5 + 0.01
        w_model(i) = 2.74 + (2.76-2.74)/(0.04) *V_c(i);
 
    elseif V_c(i) >= deadzone_nge_start && V_c(i) <= 2.5 - 0.01
        w_model(i) = 2.24 + (2.26 - 2.24) / 0.04 * V_c(i);
    else
        w_model(i) = 2.5;
    end
end

figure;
plot(w_gyro, V_c, 'o', 'MarkerEdgeColor', '#0072BD', 'MarkerFaceColor', '#0072BD', 'MarkerSize', 4);
hold on;
% plot(w_model, V_c, 'r-', 'LineWidth', 2);
xlabel('\omega_{gyro} [deg/s]');
ylabel('Command Voltage V_c [V]');
title('Inverse Property (역함수)');
% legend('Experimental Data (deg/s)', 'Linearized Model (deg/s)');
grid on;
end