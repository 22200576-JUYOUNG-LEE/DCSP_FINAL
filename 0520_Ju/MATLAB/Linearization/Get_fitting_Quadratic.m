function [coeff_pos, coeff_neg, coeff_dz, V_pos_start, V_neg_start] = Get_fitting_Quadratic(Vc, Wgyro)
    % =========================================================
    % SYSTEM PARA
    % =========================================================
    Linslope  = 1300;     % 변환 상수 K
    order     = 4;        % 정/역방향 4차 고정
    dz_order  = 1;        % 데드존 3차 고정

    % 오리지널 속도 경계값 [deg/s]
    pos_start = 10.26; %<--    
    pos_end   = 1138;   
    neg_start = -23.37; 
    neg_end   = -1128; 

    % 입력 제어 명령(vin) 기준의 경계값으로 변환
    V_pos_start = pos_start / Linslope;
    V_pos_end   = pos_end / Linslope;
    V_neg_start = neg_start / Linslope;
    V_neg_end   = neg_end / Linslope;

    % 
    vin_data = Wgyro / Linslope; 

    % (Operating Point) 
    OperPoint_Vcmd = V_pos_start;
    [~, OperPoint_idx] = min(abs(vin_data - OperPoint_Vcmd));
    OperPoint_Vc = Vc(OperPoint_idx);

    % =========================================================
    % 구간 분할 (Region Selection - vin_data 기준)
    % =========================================================
    pos_region = (vin_data >= V_pos_start) & (vin_data <= V_pos_end);
    neg_region = (vin_data >= V_neg_end) & (vin_data <= V_neg_start);
    dz_region  = (vin_data <= V_pos_start) & (vin_data >= V_neg_start); 

    % =========================================================
    % 다항식 피팅 (X축을 vin_data로 설정하여 계수 추출)
    % =========================================================
    coeff_pos = polyfit(vin_data(pos_region), Vc(pos_region), order);
    coeff_neg = polyfit(vin_data(neg_region), Vc(neg_region), order);
    coeff_dz  = polyfit(vin_data(dz_region), Vc(dz_region), dz_order); 

    % =========================================================
    % 데이터 저장 (LIN_COEF.csv)
    % =========================================================
    coeff_dz_data = nan(1, length(coeff_pos)); 
    coeff_dz_data(1:length(coeff_dz)) = coeff_dz;
    
    header_row = nan(1, length(coeff_pos));
    header_row(1:2) = [OperPoint_Vcmd, OperPoint_Vc];
    data_matrix = [header_row; coeff_pos; coeff_neg; coeff_dz_data];
    writematrix(data_matrix, '..\data\LIN_COEF.csv'); 

    % =========================================================
    % 시각화 (Plot)
    % =========================================================
    figure(); hold on; grid on; box on;
    plot(vin_data, Vc, 'b.', 'LineWidth', 1.5, 'DisplayName', 'Dataset');
    plot(vin_data(pos_region), polyval(coeff_pos, vin_data(pos_region)), 'r-', 'LineWidth', 2, 'DisplayName', 'Positive Fit');
    plot(vin_data(neg_region), polyval(coeff_neg, vin_data(neg_region)), 'm-', 'LineWidth', 2, 'DisplayName', 'Negative Fit');
    % plot(vin_data(dz_region), polyval(coeff_dz, vin_data(dz_region)), 'g-', 'LineWidth', 2, 'DisplayName', 'Deadzone Fit (3rd)');
    xlabel('Control Input v_{in}', 'FontWeight', 'bold');
    ylabel('V_c [V]', 'FontWeight', 'bold');
    legend('Location', 'best');
end