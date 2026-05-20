%% DC_PROJECT
% 3. Validation PLOT Wqyro
function Vali_plot(signal_type)
    base_dir = '../data/Validation/';

    Time = [];
    
    for idx = 1 : 23
        % 1. 동적 파일 이름 및 레전드 이름 생성
        filename = sprintf('%sVali_%s_iter%d.out', base_dir, signal_type, idx);
    
        if isfile(filename)
            % 2. Get Dataset
            Dataset = readmatrix(filename, 'NumHeaderLines', 1, 'FileType', 'text');

            if isempty(Time)
                Time = Dataset(:,1); % 첫 유효 데이터에서 시간축 획득
            end

            Vcmd(:,idx) = Dataset(:,2);
            Wgyro(:,idx) = Dataset(:,5);

        else
            % 파일이 없을 경우 경고 메시지 출력 (디버깅 용도)
            fprintf('파일을 찾을 수 없습니다: %s\n', filename);
        end
    end

    % 3. Data Processing - normalize
    Vcmd = normalize(Vcmd, 'range', [-1 1]);
    Wgyro = normalize(Wgyro, 'range', [-1 1]);

    % 3-1. MA
    window_size = 10;
    Vcmd = movmean(Vcmd, window_size, 1);  
    Wgyro = movmean(Wgyro, window_size, 1);

    % 3-2. mean 
    Vcmd_mean = mean(Vcmd, 2, 'omitnan'); % 2는 열 쪽으로 평균 낸다는 
    Wgyro_mean = mean(Wgyro, 2, 'omitnan');
    Wgyro_std  = std(Wgyro, 0, 2, 'omitnan');

    % 3-3 y axis shift
    Wgyro_mean = Wgyro_mean - 0.02;

    
    % 3. Plot Dataset (1열: Time, 6열: Wgyro)
    figure(); hold on;
    plot(Time, Vcmd_mean, 'b-', 'LineWidth', 1.5, 'DisplayName', 'V_{cmd\_mean}');

    plot(Time, Wgyro_mean, 'r-', 'LineWidth', 1.5, 'DisplayName', '\omega_{gyro\_mean}');
    plot(Time, Wgyro_mean + Wgyro_std, 'r--', 'LineWidth', 1, 'DisplayName', '+1\sigma (\omega_{gyro})');
    plot(Time, Wgyro_mean - Wgyro_std, 'r--', 'LineWidth', 1, 'DisplayName', '-1\sigma (\omega_{gyro})');

    %4. Labling
    xlabel('Time[sec]','FontWeight','bold');
    ylabel('normalized V_{cmd}[V] vs normalized \omega_{gyro}[rad/sec]','FontWeight','bold');
    
    title('Validation V_{cmd} vs \omega_{gyro}');
    legend('show', 'Location', 'best');
    hold off; grid on;

end