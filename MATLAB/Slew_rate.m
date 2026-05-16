%% DC_PROJECT: Transient Slew Rate & Wgyro Plot Integrated Analysis
clear; clc; close all;

% --- 하이패스 필터 설계 (루프 밖에서 한 번만 선언하는 것이 효율적입니다) ---
fs = 200; 
fc = 20;
hpFilter = designfilt('highpassiir', 'FilterOrder', 4, ...
    'HalfPowerFrequency', fc, 'SampleRate', fs);


% --- 1. 파라미터 및 경로 설정 ---
base_dir = '../data/Slewrate/';
% 분석할 전압 범위 설정 (0.1V 부터 1.8V까지 0.1V 간격)
v_start = 0.1;
v_step  = 0.6;
v_end   = 2.1;
Vcmd_range = v_start : v_step : v_end;

% 결과를 저장할 빈 배열 초기화
Valid_Vcmd = [];
Transient_Slopes = [];

% [수정] 정확한 Transient 분석 구간 설정 (그래프 기반)
t_trans_start = 1.995; % 신호 반응 시작 시점
t_trans_end   = 2.000; % 초기 선형 상승이 끝나는 시점

ma_window = 10; % 이동 평균 윈도우 사이즈 (필요시 조절)

% --- 2. Figure 1: Wgyro Raw Data Plot ---
figure('Name', 'Wgyro Transient Responses'); 
hold on; grid on;

for idx = Vcmd_range
    % 파일 이름 생성
    filename = sprintf('%sVcmd_%.2f_data.out', base_dir, idx);
    colName = sprintf('V_{cmd} = %.1f V', idx);
    
    if isfile(filename)
        % 1. 데이터 로드 (1열: Time, 2열: Wgyro)
        Dataset = readmatrix(filename, 'NumHeaderLines', 1, 'FileType', 'text');
        Time = Dataset(:, 1);
        Wgyro_raw = Dataset(:, 2);


        % --- 하이패스 필터 적용 ---
        Wgyro = filtfilt(hpFilter, Wgyro_raw);
        
        % 2초 이후 신호에 Moving Average (MA) 필터 적용
        idx_after_step = find(Time >= 2.0);
        if ~isempty(idx_after_step)
            Wgyro(idx_after_step) = movmean(Wgyro(idx_after_step), ma_window);
        end
        
        % 2. Data 플롯
        plot(Time, Wgyro, 'LineWidth', 1.2, 'DisplayName', colName);
    
        Valid_Vcmd(end+1, 1) = idx;
    else
        fprintf('파일을 찾을 수 없습니다: %s\n', filename);
    end
end

% Figure 1 꾸미기
xlabel('Time [sec]', 'FontWeight', 'bold');
ylabel('\omega_{gyro} [rad/sec]', 'FontWeight', 'bold');
title('Transient Response of \omega_{gyro} (0.1V ~ 1.8V)', 'FontSize', 12);
legend('show', 'Location', 'best');
hold off;

% --- 3. Figure 2: Transient Slew Rate 분석 (Vcmd vs Slope) ---
if ~isempty(Valid_Vcmd)
    % Vcmd에 따른 기울기(Slope)의 변화율(Slew Rate) 계산
    p_slew = polyfit(Valid_Vcmd, Transient_Slopes, 1);
    slew_rate = p_slew(1);
    
    % 커맨드 창에 결과 출력
    fprintf('\n========================================\n');
    fprintf('계산된 Transient Slew Rate: %.4f [rad/s^2 / V]\n', slew_rate);
    fprintf('========================================\n');
    
    % Figure 2 생성
    figure('Name', 'Transient Slew Rate Analysis');
    hold on; grid on;
    
    % 측정된 기울기 데이터 플롯
    plot(Valid_Vcmd, Transient_Slopes, 'bo-', 'LineWidth', 1.5, 'MarkerFaceColor', 'b', 'DisplayName', 'Experimental Slope');
    
    % 피팅된 선형 그래프 플롯
    fit_line = polyval(p_slew, Valid_Vcmd);
    plot(Valid_Vcmd, fit_line, 'r--', 'LineWidth', 2, 'DisplayName', sprintf('Linear Fit (Gain: %.2f)', slew_rate));
    
    % Figure 2 꾸미기
    xlabel('V_{cmd} [V]', 'FontSize', 12, 'FontWeight', 'bold');
    ylabel('Transient Slope [rad/s^2]', 'FontSize', 12, 'FontWeight', 'bold');
    title('Transient Slew Rate Analysis', 'FontSize', 14);
    legend('show', 'Location', 'northwest');
    hold off;
else
    fprintf('\n[안내] 유효한 데이터 파일이 없어 Slew Rate를 계산할 수 없습니다. 경로를 확인해주세요.\n');
end