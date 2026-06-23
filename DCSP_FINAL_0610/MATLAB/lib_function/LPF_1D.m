function X_LPF = LPF_1D(Time_uniform, X, Wc)

    sampling_period                 = 0.005;
    [CT_LPF_NUM, CT_LPF_DEN]        = butter(2, Wc, 'low', 's');

    % analog
    Hs                              = tf(CT_LPF_NUM, CT_LPF_DEN); % CT transfer function
    
    % digital
    Hz_tustin                       = c2d(Hs, sampling_period, 'tustin');
    
    X_LPF                           = lsim(Hz_tustin, X, Time_uniform);
end