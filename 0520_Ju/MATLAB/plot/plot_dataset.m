%% DC_PROJECT
% 1. PLOT DATASET
function plot_dataset(filename)
    
    %1. Get label on first line
    fid = fopen(filename, 'r');
    firstLine = fgetl(fid);
    fclose(fid);
    Labels = strsplit(strtrim(firstLine));

    %2. Get Dataset
    Dataset =  readmatrix(filename, 'NumHeaderLines', 1, 'FileType','text');
    [~, Dataset_col] = size(Dataset);

    %3. Plot Dataset
    figure(); hold on;
    for idx=2:Dataset_col

        colName = Labels{idx};% at using array use {}

        plot(Dataset(:,1),Dataset(:,idx),'o-','LineWidth',1.2, 'DisplayName', colName);

         %4. Labling
        xlabel(Labels{1},'FontWeight','bold');
        ylabel(Labels{idx},'FontWeight','bold');
    end
    hold off;

    title("Motor Static Property", 'FontWeight','bold');
    
    legend('Location','best');

   

    grid on;

end