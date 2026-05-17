%% DC_PROJECT
% 2. PLOT Wqyro
function plot_Wgyro(base_dir)
    
    %3. Plot Dataset
    figure(); hold on;
    
    for idx = 1.0 : 0.1 : 1.8
    
        % 1. 동적 파일 이름 및 레전드 이름 생성
        filename = sprintf('../data/%s/Vcmd_%.2f_data.out', base_dir, idx);
        colName = sprintf('V_{cmd} = %.1f V', idx);
    
        if isfile(filename)
            % 2. Get Dataset
            Dataset = readmatrix(filename, 'NumHeaderLines', 1, 'FileType', 'text');

            Wgyro = Dataset(:,2);
            window_size = 10;
            Wgyro = movmean(Wgyro, window_size, 1);
            
            % 3. Plot Dataset (1열: Time, 6열: Wgyro)
            plot(Dataset(:,1), Wgyro, 'LineWidth', 1.2, 'DisplayName', colName);
        end
        
    end
     %4. Labling
    xlabel('Time[sec]','FontWeight','bold');
    ylabel('\omega_{gyro}[rad/sec]','FontWeight','bold');
    
    legend('show', 'Location', 'best');
    hold off; grid on;

end
