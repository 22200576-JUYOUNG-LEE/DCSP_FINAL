function X_BPF = BPF_1D(Time_uniform, X, PassBand)

    sampling_period                 = 0.005;
    X_range                         = [X-PassBand/2; X+PassBand/2]; 
    [CT_LPF_NUM, CT_LPF_DEN]        = butter(1, X_range, 'bandpass', 's');

    % analog
    Hs                              = tf(CT_LPF_NUM, CT_LPF_DEN); % CT transfer function
    
    % digital
    Hz_tustin                       = c2d(Hs, sampling_period, 'tustin');
    
    X_BPF                           = lsim(Hz_tustin, X, Time_uniform);
end