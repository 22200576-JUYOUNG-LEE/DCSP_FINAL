%% DC_PROJECT
% 1. PLOT DATASET
function plot_sweep(filename)
    
    %1. Get label on first line
    fid = fopen(filename, 'r');
    firstLine = fgetl(fid);
    fclose(fid);
    Labels = strsplit(strtrim(firstLine));

    %2. Get Dataset
    Dataset =  readmatrix(filename, 'NumHeaderLines', 1, 'FileType','text');

    %3. Plot Dataset
    figure(); hold on;

    colName = Labels{2};% at using array use {}

    plot(Dataset(:,1),Dataset(:,2),'o-','LineWidth',1.2, 'DisplayName', colName);

     %4. Labling
    xlabel(Labels{1},'FontWeight','bold');
    ylabel(Labels{2},'FontWeight','bold');

    hold off;

    title("Motor Static Property", 'FontWeight','bold');
    
    legend('Location','best');
    grid on;

end