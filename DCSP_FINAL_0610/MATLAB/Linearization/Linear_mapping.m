function Linear_mapping(filename, Vc, Wgyro)
% DATA 로딩
data = readmatrix(filename, 'NumHeaderLines', 1, "FileType", "text");
V_c    = data(:,1);
w_gyro = data(:,2);

Linslope = 1300;

% Coefficients as
[coeff_pos, coeff_neg, coeff_dz, V_pos_start, V_neg_start] = Get_fitting_Quadratic(Vc, Wgyro);

% 포화 한계값도 vin 기준으로 변환
MAX_RATE = 1400 / Linslope; 

Vc_model = zeros(size(w_gyro));

% ============================================================
% Linear mapping 
% ============================================================
for i = 1:length(w_gyro) 
    
    % 입력 속도 요소를 제어 입력 v_in으로 변환
    v_in = w_gyro(i) / Linslope;
    
    % SATURATION (vin 기준 포화 제한)
    if v_in > MAX_RATE
        v_in = MAX_RATE;
    elseif v_in < -MAX_RATE
        v_in = -MAX_RATE;
    end

    % 1. 정방향 구간 (vin 직접 대입)
    if v_in >= V_pos_start
        Vc_model(i) = polyval(coeff_pos, v_in);
    % 2. 역방향 구간
    elseif v_in <= V_neg_start
        Vc_model(i) = polyval(coeff_neg, v_in);
    % 3. 데드존 구간 (3차 피팅)
    else
        Vc_model(i) = polyval(coeff_dz, v_in);
    end
end

% ============================================================
% 시각화
% ============================================================
figure(); hold on; grid on; box on;
plot(w_gyro, V_c, 'b.', 'LineWidth', 1.5, 'DisplayName', 'Experimental Data');
plot(w_gyro, Vc_model, 'r-', 'LineWidth', 2, 'DisplayName', 'Mapped Model');
xlabel('\omega_{gyro} [deg/sec]', 'FontWeight', 'bold');
ylabel('V_c [V]', 'FontWeight', 'bold');
title('Polynomial Mapping : V_c = f(v_{in})');
legend('Location', 'best');
end