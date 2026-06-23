function [freqs, gains, phases] = generate_bode_plot(data_dir)

    
    sampling_freq=200.0; 
    
    addpath('../lib_function');

    search_freqs = 0.3:0.01:1.0; 
    
    valid_freqs = [];
    gains_raw = [];
    phase_deg = [];
    
    for f = search_freqs
        fn = sprintf('%sBodeMag_sin_freq%.2f.out', data_dir, f);
        if ~isfile(fn), continue; end
        
        data = readmatrix(fn, 'FileType', 'text', 'NumHeaderLines', 1);
        t = data(1000:end, 1);
        u = data(1000:end, 2); 
        y = data(1000:end, 4); 
        w = 2 * pi * f;
        H = [sin(w*t), cos(w*t), ones(size(t))];
        
        % Curve Fitting 
        c_in = H \ u;
        A_in = norm(c_in(1:2));
        phi_in = atan2(c_in(2), c_in(1));
        
        c_out = H \ y;
        A_out = norm(c_out(1:2));
        phi_out = atan2(c_out(2), c_out(1));
        
        if A_in > 1e-6
            valid_freqs = [valid_freqs; f];
            gains_raw = [gains_raw; A_out / A_in];
            ph = (phi_out - phi_in) * 180 / pi;
            phase_deg = [phase_deg; mod(ph + 180, 360) - 180];
            
            %
            % The script is added: 06.23.26 Good
            %
            if ismember(f, [0.3, 0.5, 0.72, 0.8, 0.90, 1.4])
                
                % After Curve fit 
                u_fit = A_in * sin(w*t + phi_in) + c_in(3);
                y_fit = A_out * sin(w*t + phi_out) + c_out(3);
                
                % FFT 
                [Freq, FFT_Mag_u] = FFT_1D(t, [u, u_fit], sampling_freq); % 1 column: Raw, 2 column: Fitted
                [~,    FFT_Mag_y] = FFT_1D(t, [y, y_fit], sampling_freq); 
                
                % Plot
                figure('Name', sprintf('FFT analysis (f = %.2f Hz)', f), 'NumberTitle', 'off');
                
                              
                % outputs
                plot(Freq, FFT_Mag_y(:, 1), 'r-', 'LineWidth', 2); hold on;
                plot(Freq, FFT_Mag_y(:, 2), 'b', 'LineWidth', 1.5); hold off;
                xlim([0, 100]); grid on;
                xlabel('Frequency (Hz)'); ylabel('Magnitude');
                title(sprintf('Output Signal (y) FFT at %.2f Hz', f));
                legend('Raw Data', 'Curve Fitted');
            end
            
        end
    end
    
   
    freqs = valid_freqs;
    gains = gains_raw;
    phases = phase_deg;


    smoothed_phases = smooth(phases, 5, 'moving');


    figure('Name', 'Experimental Phase Response (Filtered)', 'NumberTitle', 'off');
    
    plot(freqs, phases, 'm--', 'LineWidth', 1); hold on;
    
    
    plot(freqs, smoothed_phases, 'b-o', 'LineWidth', 2, 'MarkerFaceColor', 'b', 'MarkerSize', 4); 
    hold off;
    
    grid on;
    xlabel('Frequency (Hz)'); 
    ylabel('Phase (Degree)');
    title('Actual Phase Plot for Motor Modeling (Raw vs Smoothed)');
    legend('Raw Phase', 'Smoothed Phase ');
    
    ylim([min(phases)-5, max(phases)+5]);
    

    % Update the phases: 
    phases=smoothed_phases;


end