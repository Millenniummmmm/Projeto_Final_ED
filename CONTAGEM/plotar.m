% Ler o arquivo
dados = readtable('dados_comparacao.csv');

% Ordenar pela coluna 'Procurado'
dados = sortrows(dados, 'Procurado');

% Dados
amostra = dados(1:200, :);
filename = 'comparacoes_animado.gif';

% Figura inicial
figure;

for i = 1:height(amostra)
    clf;  % Limpa a figura

    % ==== SUBPLOT 1: ABB ====
    subplot(2,1,1);
    scatter(amostra.Procurado(1:i), amostra.Comparacoes_ABB(1:i), 30, 'b', 'filled');
    hold on;

    % Media ABB
    media_abb = mean(amostra.Comparacoes_ABB(1:i));
    yline(media_abb, '--black', sprintf('Média ABB = %.1f', media_abb), ...
          'LabelHorizontalAlignment', 'left', 'LineWidth', 1.5);
    title(sprintf('ABB - Comparações até %d valores', i));
    
    %ylabel('Comparações (log)');
    %set(gca, 'YScale', 'log');
    grid on;

    % ==== SUBPLOT 2: Lista Encadeada ====
    subplot(2,1,2);
    scatter(amostra.Procurado(1:i), amostra.Comparacoes_Lista(1:i), 30, 'r', 'filled');
    hold on;
    
    % Media lista
    media_lista = mean(amostra.Comparacoes_Lista(1:i));
    yline(media_lista, '--black', sprintf('Média Lista = %.1f', media_lista), ...
          'LabelHorizontalAlignment', 'left', 'LineWidth', 1.5);
    title(sprintf('Lista Encadeada - Comparações até %d valores', i));
    xlabel('Valor Procurado');
    
    %ylabel('Comparações (log)');
    %set(gca, 'YScale', 'log');
    grid on;

    % Ajustes finais e captura do frame
    drawnow;
    frame = getframe(gcf);
    im = frame2im(frame);
    [A,map] = rgb2ind(im,256);

    % Escreve no GIF
    if i == 1
        imwrite(A, map, filename, 'gif', 'LoopCount', Inf, 'DelayTime', 0);
    else
        imwrite(A, map, filename, 'gif', 'WriteMode', 'append', 'DelayTime', 0);
    end
end
