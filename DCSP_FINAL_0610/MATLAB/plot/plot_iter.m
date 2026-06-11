%% DC_PROJECT
function [X_mean, Y_mean]=plot_iter(filename, X, Y, itr)
    
    for idx = 1 : itr
        current_file = sprintf('%s%d.out', filename, idx);
    
        if isfile(current_file)
            % 2. Get Dataset
            Dataset = readmatrix(current_file, 'NumHeaderLines', 1, 'FileType', 'text');
            Vc(:,idx) = Dataset(:,X);
            Wgyro(:,idx) = Dataset(:,Y) .* 0.017453;
        else

            fprintf('Cannot find a file: %s\n', current_file);  
        end
    end
    % 3-2. mean 
    Wgyro_mean = mean(Wgyro, 2, 'omitnan');
    Wgyro_std  = std(Wgyro, 0, 2, 'omitnan');
    
    % 3. Plot Dataset
    figure(); hold on;
    plot(mean(Vc, 2, 'omitnan'), Wgyro_mean, 'bo', 'LineWidth', 1.5, 'DisplayName', '\omega_{gyro\_mean}');
    
    % 4. Labeling 
    xlabel('V_{cmd\_mean} [V]','FontWeight','bold');
    ylabel('\omega_{gyro\_mean} [deg/sec]', 'FontWeight','bold');
    
    title('Static_Property');
    legend('show', 'Location', 'best');
    grid on;
    hold off;
    
    fprintf("Standard Deviation: %.4f\n", mean(Wgyro_std, 'omitnan')); 

    X_mean=mean(Vc,2,'omitnan');
    Y_mean=mean(Wgyro, 2, 'omitnan'); 

end