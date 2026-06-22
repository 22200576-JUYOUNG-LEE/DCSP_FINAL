function [freqs, gains, phases] = generate_bode_plot(data_dir)
    % Define frequency range as per your files
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
        end
    end
    
    freqs = valid_freqs;
    gains = gains_raw;
    phases = phase_deg;

end