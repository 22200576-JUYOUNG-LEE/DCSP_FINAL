%% DC_PROJECT
% 2. PLOT Wgyro & Normalized Bode Magnitude Plot (dB Scale)
function plot_BodeMag_dB(base_dir)
    
    % 분석할 주파수 범위 설정 (첨부된 데이터 기준 0.0 ~ 1.6)
    freq_range = 1.0 : 0 : 1.3; 
    
    Valid_Freqs = [];
    Magnitudes = [];
    
    % --- 1. 시영역 응답 플롯 ---
    figure(); hold on; grid on;
    
    for idx = freq_range
    
        % 1. 동적 파일 이름 및 레전드 이름 생성
        filename = sprintf('../data/%s/BodeMag_sin_freq%.2f.out', base_dir, idx);
        colName = sprintf('Freq = %.1f Hz', idx);
    
        if isfile(filename)
            % 2. 데이터 로드
            Dataset = readmatrix(filename, 'NumHeaderLines', 1, 'FileType', 'text');
            Time = Dataset(:,1);
            Wgyro = Dataset(:,5);
            
            % 노이즈 제거 (이동 평균)
            window_size = 10;
            Wgyro = movmean(Wgyro, window_size, 1);
            
            % 3. 시영역 플롯
            plot(Time, Wgyro, 'LineWidth', 1.2, 'DisplayName', colName);
            
            % --- 보데 플롯을 위한 최대 크기 추출 ---
            % 0 Hz는 진동이 없어 dB 계산 시 -Inf가 되므로 제외 (0.2Hz부터 추출)
            if idx > 0 
                active_idx = find(Time >= 1.0 & Time <= 5.0);
                if ~isempty(active_idx)
                    active_Wgyro = Wgyro(active_idx);
                    
                    % (최대값 - 최소값) / 2 를 하여 순수한 사인파의 진폭(Amplitude)을 구함
                    amp_out = (max(active_Wgyro) - min(active_Wgyro)) / 2;
                    
                    Valid_Freqs(end+1) = idx;
                    Magnitudes(end+1) = amp_out;
                end
            end
        else
            fprintf('[안내] 파일을 찾을 수 없습니다: %s\n', filename);
        end
        
    end
    
    % 시영역 그래프 꾸미기
    xlabel('Time [sec]', 'FontWeight', 'bold');
    ylabel('\omega_{gyro} [rad/sec]', 'FontWeight', 'bold');
    title('Time Domain Sine Responses', 'FontSize', 12);
    legend('show', 'Location', 'best');
    hold off;
    
    % --- 2. 보데 크기 선도 (Normalized Bode Magnitude Plot - dB Scale) 생성 ---
    if ~isempty(Valid_Freqs)
        figure('Name', 'Bode Magnitude Plot (Normalized dB)'); hold on; grid on;
        
        % [수정됨] 0.2Hz(첫 번째 유효 주파수)의 크기를 기준으로 정규화 수행
        % Magnitudes(1) 값으로 모든 값을 나누어 첫 포인트가 0 dB에서 시작하게 됨
        mag_dB = 20 * log10(Magnitudes / Magnitudes(1));
        
        % 선형 주파수 간격이므로 일반 plot 사용
        plot(Valid_Freqs, mag_dB, 'ro-', 'LineWidth', 1.5, 'MarkerFaceColor', 'r');
        
        xlabel('Frequency [Hz]', 'FontSize', 12, 'FontWeight', 'bold');
        ylabel('Normalized Magnitude [dB]', 'FontSize', 12, 'FontWeight', 'bold');
        title('Normalized Bode Magnitude Plot', 'FontSize', 14);
        
        hold off;
    end
end