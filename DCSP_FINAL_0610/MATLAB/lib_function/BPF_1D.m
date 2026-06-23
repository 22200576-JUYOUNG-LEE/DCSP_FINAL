function X_BPF = BPF_1D(Time, X, Wc, PassBand)

    time_length     = length(Time);
    time_uniform    = linspace(Time(1), Time(end), time_length);
    sampling_period                 = Time(2) - Time(1);

    Wc_range                        = [Wc-PassBand/2; Wc+PassBand/2]; 
    [CT_BPF_NUM, CT_BPF_DEN]        = butter(1, Wc_range, 'bandpass', 's');

    % analog
    Hs                              = tf(CT_BPF_NUM, CT_BPF_DEN); % CT transfer function
    
    % digital
    Hz_tustin                       = c2d(Hs, sampling_period, 'tustin');
    
    X_BPF                           = lsim(Hz_tustin, X, time_uniform);
end