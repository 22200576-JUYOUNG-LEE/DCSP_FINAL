% subject: Digital control and signal Processing
% title: Linearization Validation function
% author: Juyoung Lee

function Validation(signal_type)
    base_dir        = '../data/Validation/';
    scale_rad2deg   = 180.0/pi ; 
    
    Time        = [];
    Wcmd        = [];
    Wgyro       = [];

    for idx = 1 : 23
        % -----------------------------------------------------------------
        % 1. Import Data
        % -----------------------------------------------------------------
        filename        = sprintf('%sValid_%s_iter%d.out', base_dir, signal_type, idx);
    
        if isfile(filename)
            Dataset     = readmatrix(filename, 'NumHeaderLines', 1, 'FileType', 'text');

            if isempty(Time)
                Time    = Dataset(:,1);
            end

            Wcmd(:,idx)     = Dataset(:,2).*scale_rad2deg;
            Wgyro(:,idx)    = Dataset(:,4).*scale_rad2deg;

        else
            % fprintf('파일을 찾을 수 없습니다: %s\n', filename); 
        end
    end
% -------------------------------------------------------------------------
% 2. Data Processing
% -------------------------------------------------------------------------
    % 2-1. normalize
    % Wcmd        = normalize(Wcmd, 'range', [-24*scale_rad2deg 24*scale_rad2deg]);

    % 2-2. Movemean
    window_size = 10;
    Wcmd        = movmean(Wcmd, window_size, 1);  
    Wgyro       = movmean(Wgyro, window_size, 1);

    % 2-3. mean 
    Vcmd_mean   = mean(Wcmd, 2, 'omitnan'); % 2는 열 쪽으로 평균 낸다는 
    Wgyro_mean  = mean(Wgyro, 2, 'omitnan');
    % Wgyro_std   = std(Wgyro, 0, 2, 'omitnan');

% -------------------------------------------------------------------------
% 3. Plot Cmd & Output
% -------------------------------------------------------------------------
    figure(); hold on;
    plot(Time, Vcmd_mean, 'b-', 'LineWidth', 1.5, 'DisplayName', 'V_{cmd\_mean}');

    plot(Time, Wgyro_mean, 'r-', 'LineWidth', 1.5, 'DisplayName', '\omega_{gyro\_mean}');
    % plot(Time, Wgyro_mean + Wgyro_std, 'r--', 'LineWidth', 1, 'DisplayName', '+1\sigma (\omega_{gyro})');
    % plot(Time, Wgyro_mean - Wgyro_std, 'r--', 'LineWidth', 1, 'DisplayName', '-1\sigma (\omega_{gyro})');

    xlabel('Time[sec]','FontWeight','bold');
    ylabel('\omega_{cmd}[rad/sec] vs \omega_{gyro}[deg/sec]','FontWeight','bold');
    
    title('Validation \omega_{cmd} [rad/sec]vs \omega_{gyro}[rad/sec]');
    legend('show', 'Location', 'best');
    hold off; grid on;
 
end