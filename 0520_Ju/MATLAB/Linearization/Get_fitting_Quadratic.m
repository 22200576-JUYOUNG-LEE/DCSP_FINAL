function [coeff_pos, coeff_neg, coeff_dz, V_pos_start, V_neg_start] = Get_fitting_Quadratic(Vc, Wgyro)
    % =========================================================
    % SYSTEM PARA
    % =========================================================
    Linslope  = 1300;     % 변환 상수 K
    order     = 4;        % 정/역방향 4차 고정
    dz_order  = 3;        % 데드존 3차 고정

    % 오리지널 속도 경계값 [deg/s]
    pos_start = 57.2428; %<--    
    pos_end   = 1135;   
    neg_start = -76.1736; 
    neg_end   = -1120; 

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

    % ========================================================
    % 2 point 의 기울기, 좌표값으로 3차함수 구하기
    % ========================================================
    x1 = V_neg_start;
    x2 = V_pos_start;

    % 2. 연속성을 위해 인접 피팅 곡선에서 y값 추출
    y1 = polyval(coeff_neg, x1);
    y2 = polyval(coeff_pos, x2);
    
    % 3. 인접 곡선의 다항식을 미분하여 경계에서의 기울기(m) 계산
    d_coeff_neg = polyder(coeff_neg);
    d_coeff_pos = polyder(coeff_pos);
    m1 = polyval(d_coeff_neg, x1);
    m2 = polyval(d_coeff_pos, x2);
    
    % 4. 3차 다항식 결정을 위한 행렬 방정식 구성 (A * p = B)
    A = [ x1^3,   x1^2,   x1, 1; ...
          x2^3,   x2^2,   x2, 1; ...
         3*x1^2, 2*x1,     1, 0; ...
         3*x2^2, 2*x2,     1, 0 ];
     
    B = [y1; y2; m1; m2];
    
    % 5. 계수 [a, b, c, d] 계산 후 polyval 규격(행벡터)에 맞춰 변환
    coeff_dz = (A \ B)';


    % coeff_dz  = polyfit(vin_data(dz_region), Vc(dz_region), dz_order); 

    % =========================================================
    % 데이터 저장 (LIN_COEF.csv)
    % =========================================================
    coeff_dz_data = nan(1, length(coeff_pos)); 
    coeff_dz_data(1:length(coeff_dz)) = coeff_dz;
    
    header_row = nan(1, length(coeff_pos));
    header_row(1:2) = [OperPoint_Vcmd, OperPoint_Vc];
    data_matrix = [header_row; coeff_pos; coeff_neg; coeff_dz_data];
    writematrix(data_matrix, '..\data\LIN_COEF.csv'); 


    % ========================================================
    % 그래프 한번에 띄우기
    % ========================================================
    Vc_fit = nan(size(vin_data));
    % 각 논리 배열(Region) 위치에 맞는 피팅 값 대입
    Vc_fit(pos_region) = polyval(coeff_pos, vin_data(pos_region));
    Vc_fit(neg_region) = polyval(coeff_neg, vin_data(neg_region));
    Vc_fit(dz_region)  = polyval(coeff_dz, vin_data(dz_region));

    [vin_sorted, sort_idx] = sort(vin_data);
    Vc_fit_sorted          = Vc_fit(sort_idx);

    % =========================================================
    % 시각화 (Plot)
    % =========================================================
    figure(); hold on; grid on; box on;
    plot(vin_data, Vc, 'b.', 'LineWidth', 1.5, 'DisplayName', 'Dataset');
    % plot(vin_data(pos_region), polyval(coeff_pos, vin_data(pos_region)), 'r-', 'LineWidth', 2, 'DisplayName', 'Positive Fit');
    % plot(vin_data(neg_region), polyval(coeff_neg, vin_data(neg_region)), 'm-', 'LineWidth', 2, 'DisplayName', 'Negative Fit');
    % plot(vin_data(dz_region), polyval(coeff_dz, vin_data(dz_region)), 'g-', 'LineWidth', 2, 'DisplayName', 'Deadzone Fit (3rd)');
    plot(vin_data, Vc_fit, 'r-', 'LineWidth', 2, 'DisplayName', 'Total Piecewise Fit');
    xlabel('Control Input v_{in}', 'FontWeight', 'bold');
    ylabel('V_c [V]', 'FontWeight', 'bold');
    legend('Location', 'best');
end