function X_BPF = BPF_1D(Time_uniform, X, Wc, PassBand)

    sampling_period                 = 0.005;
    Wc_range                        = [Wc-PassBand/2; Wc+PassBand/2]; 
    [CT_BPF_NUM, CT_BPF_DEN]        = butter(1, Wc_range, 'bandpass', 's');

    % analog
    Hs                              = tf(CT_BPF_NUM, CT_BPF_DEN); % CT transfer function
    
    % digital
    Hz_tustin                       = c2d(Hs, sampling_period, 'tustin');
    
    X_BPF                           = lsim(Hz_tustin, X, Time_uniform);
end