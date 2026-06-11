function [CoefP, CoefN] = LIN_getCoef(Motor_Volt, Rate_Gyro, idxN_SAT, idxN_DZ, idxP_DZ, idxP_SAT)
    % =========================================================
    % SYSTEM PARA
    % =========================================================
    Linslope  = 21.365;     % 변환 상수 K
    order     = 4;        % 정/역방향 4차 고정

    % 오리지널 속도 경계값 [deg/s]
    pos_start = 0.3325; 
    pos_end   = 19.8222;   
    neg_start = -0.5112; 
    neg_end   = -19.7323; 

    % 입력 제어 명령(vin) 기준의 경계값으로 변환
    V_pos_start = pos_start / Linslope;
    V_pos_end   = pos_end / Linslope;
    V_neg_start = neg_start / Linslope;
    V_neg_end   = neg_end / Linslope;

    % 
    vin_data = Rate_Gyro / Linslope; 

    % (Operating Point) 
    OperPoint_Vcmd = V_pos_start;
    [~, OperPoint_idx] = min(abs(vin_data - OperPoint_Vcmd));
    OperPoint_Vc = Motor_Volt(OperPoint_idx);

    pos_region = (vin_data >= V_pos_start) & (vin_data <= V_pos_end);
    neg_region = (vin_data >= V_neg_end) & (vin_data <= V_neg_start);

    CoefP = polyfit(vin_data(pos_region), Motor_Volt(pos_region), order);
    CoefN = polyfit(vin_data(neg_region), Motor_Volt(neg_region), order);

    % =========================================================
    % Save Data (..\data\LINEAR_COEF.csv)
    % =========================================================
    header_row = nan(1, length(CoefP));
    header_row(1:2) = [OperPoint_Vcmd, OperPoint_Vc];
    data_matrix = [header_row; coeff_pos; coeff_neg];
    writematrix(data_matrix, '..\data\LINEAR_COEF.csv'); 

end