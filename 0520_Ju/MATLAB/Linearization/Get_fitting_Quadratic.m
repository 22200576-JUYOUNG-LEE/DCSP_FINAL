%% DC_PROJECT
% 1. PLOT DATASET
function [coeff_pos, coeff_neg] = Get_fitting_Quadratic(filename)

    data=readmatrix(filename, 'NumHeaderLines', 1, "FileType","text");

    Vc=data(:,1); %[V] 
    Wgyro=data(:,2); %[deg/sec]

<<<<<<< Updated upstream
    pos_region = (Vc >= 2.70) & (Vc <= 4.00); 
=======
    pos_region = (Vc >= 2.7) & (Vc <= 4.0); 
>>>>>>> Stashed changes

    Vc_pos=Vc(pos_region);
    Wgyro_pos=Wgyro(pos_region); 

    A_pos=[Vc_pos.^2, Vc_pos, ones(size(Vc_pos))];
    B_pos=Wgyro_pos; 

    coeff_pos = A_pos \ B_pos; 


<<<<<<< Updated upstream
    neg_region = (Vc >= 1.00) & (Vc <= 2.30);
=======
    neg_region = (Vc >= 1.0) & (Vc <= 2.3);
>>>>>>> Stashed changes
    Vc_neg = Vc(neg_region);
    Wgyro_neg = Wgyro(neg_region);

    A_neg = [Vc_neg.^2, Vc_neg, ones(size(Vc_neg))];
    B_neg = Wgyro_neg;

coeff_neg = A_neg \ B_neg;

% conclusion: 
fprintf('Positive region: W = %.4f*V^2 + %.4f*V + %.4f\n', coeff_pos);
fprintf('Negative region: W = %.4f*V^2 + %.4f*V + %.4f\n', coeff_neg);
    

end