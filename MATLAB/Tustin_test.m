% Tustin 

v_start = 0.1;
v_step  = 0.6;
v_end   = 3.2;
Vcmd_range = v_start : v_step : v_end;

% 결과를 저장할 빈 배열 초기화
Valid_Vcmd = [];
Transient_Slopes = [];

Hz_tustin = HPF_tustin();

% --- 1. 파라미터 및 경로 설정 ---
base_dir = '../data/Slewrate/';

% 파일 이름 생성
filename = sprintf('%sVcmd_%.2f_data.out', base_dir, 1.00);
colName = sprintf('V_{cmd} = %.1f V', idx);

figure('Name', 'Wgyro Transient Responses'); hold on; grid on;
    
for idx = Vcmd_range
    if isfile(filename)
        % 1. 데이터 로드 (1열: Time, 2열: Wgyro)
        Dataset = readmatrix(filename, 'NumHeaderLines', 1, 'FileType', 'text');
        Time = Dataset(:, 1);
        Wgyro_raw = Dataset(:, 2);
    
        % --- 하이패스 필터 적용 ---
        [y, t_out] = lsim(Hz_tustin, Wgyro_raw, Time_fixed);
        
        % % 2초 이후 신호에 Moving Average (MA) 필터 적용
        % idx_after_step = find(Time >= 2.0);
        % if ~isempty(idx_after_step)
        %     Wgyro(idx_after_step) = movmean(Wgyro(idx_after_step), ma_window);
        % end
        % 
        
        % 2. Data 플롯
        plot(Time, Wgyro, 'LineWidth', 1.2, 'DisplayName', colName);
    
        Valid_Vcmd(end+1, 1) = idx;
    else
        fprintf('파일을 찾을 수 없습니다: %s\n', filename);
    end
end






