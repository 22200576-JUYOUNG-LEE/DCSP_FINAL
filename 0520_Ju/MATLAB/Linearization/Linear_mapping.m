% 1. 원본 실험 데이터 로드 (V_c, w_gyro)
function Linear_mapping(filename)


data=readmatrix(filename, 'NumHeaderLines', 1, "FileType","text");

V_c=data(:,1); 
w_gyro=data(:,2); 

[coeff_pos, coeff_neg, pos_start, neg_start] = Get_fitting_Quadratic(filename);

% . 선형화 모델 파라미터 할당
a = coeff_pos(1);   b = coeff_pos(2);   c = coeff_pos(3);    % Positive region
a1 = coeff_neg(1);  b1 = coeff_neg(2);  c1 = coeff_neg(3);   % Negative region



% 3. 모델 출력 계산 및 단위 변환 (rad/s -> deg/s)
w_model=zeros(size(w_gyro)); 

for i = 1:length(V_c)
    if V_c(i) > pos_start % 양의 유효 구간
        w_model(i) = a*(V_c(i))^2 + b*V_c(i) + c;
    elseif V_c(i) < neg_start % 음의 유효 구간
        w_model(i) = a1*(V_c(i))^2 + b1*V_c(i) + c1;
    else
        w_model(i) = 0; % 데드존
    end
end

% 4. 데이터 플롯 및 비교
figure;
plot(V_c, w_gyro, 'o', 'MarkerEdgeColor', '#0072BD', 'MarkerFaceColor', '#0072BD', 'MarkerSize', 4);
hold on;
plot(V_c, w_model, 'r-', 'LineWidth', 2);
xlabel('Command Voltage V_c [V]');
ylabel('\omega_{gyro} [deg/s]');
title('Motor Static Property vs Linearized Model');
legend('Experimental Data (deg/s)', 'Linearized Model (deg/s)');
grid on;

figure;
plot( w_gyro,V_c, 'o', 'MarkerEdgeColor', '#0072BD', 'MarkerFaceColor', '#0072BD', 'MarkerSize', 4);
hold on;
plot( w_model,V_c, 'r-', 'LineWidth', 2);
xlabel('Command Voltage V_c [V]');
ylabel('\omega_{gyro} [deg/s]');
title('역함수');
legend('Experimental Data (deg/s)', 'Linearized Model (deg/s)');
grid on;
