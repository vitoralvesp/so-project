# Simulação de Elevador
Este projeto se baseia na simulação de um elevador em um prédio de 6 andares, sendo que cada andar é considerado que pessoas estão fazendo requisições constantes do elevador. Por isso, o programa é dividido nas seguintes etapas:
- **Duas Threads Principais:** consideramos uma thread para as pessoas que realizam a requisição do elevador em um andar e uma thread para processar o progresso do elevador até o andar de destino/chamada;
- **Sincronização:**
  - Função request: utiliza mutexes neste escopo para gerar um número aleatório até 6 (total de andares do prédio) e atualizar o atributo de chamada do elevaor;
  - Função move_elevator: utiliza mutexes neste escopo para mover o elevador até o andar de destino, incrementando ou decrementando o atributo current_floor.
- **Comunicação entre as Threads:** as threads estão em comunicação de modo que a requisição é realizada sempre antes do elevador começar a se mover.
