# 3. MATERIAIS E MÉTODOS

## Microcontrolador PIC 16F877A
<div style="text-align: center; padding: 10px;">
  <img src="img/pic16f877a.png" width="50%" alt="PIC 16F877A">
</div>

### Características
- Sua frequência de operação chega a 20MHz, sendo utilizado 4MHz;
- Sua velocidade de processamento é de 200ns por ciclo de instrução;
- Possui 40 pinos com 5 portas (A, B, C, D, E) e 33 pinos I/O, sendo que 5 pinos, da porta A, podem ser analógicos;
- Contém baixo consumo de energia, com o Flash da memória EEPROM de alta velocidade e tecnologia.
### Memórias
- Até 8Kb por 14 palavras de memória flash do programa;
- Até 368 por 8 bytes de memória de dados (RAM);
- Até 256 por 8 bytes de memória de dados EEPROM.
### Alimentação
- Sua faixa de tensão de funcionamento vai de 2,0V a 5,5V.
### Periféricos
- Possui 100.000 ciclos de leitura/escrita da memória Flash de programa;
- Possui 1.000.000 ciclos de leitura/escrita de dados da memória EEPROM;
- Retém os dados da memória EEPROM por até 40 anos;
- 4 Temporizadores/Contadores;
- Possibilita interrupção externa através de pinos do microcontrolador.

## Display LCD 16x2
<div style="text-align: center; padding: 10px;">
  <img src="img/display16x2.png" width="50%" alt="Display LCD 16x2">
</div>

## Buzzer
<div style="text-align: center; padding: 10px;">
  <img src="img/buzzer.png" width="30%" alt="Buzzer">
</div>

## Módulo Wireless NRF24L01
### Características
- Acompanha uma antena embutida que opera na frequência de operação de 2,4GHz;
- Sua velocidade de operação vai até 2Mbps;
- Modulação GFSK;
- Comunicação multiponto de 126 canais;
- Possui 8 pinos: GND, VCC, CE, CSN, SCK, MOSI, MISO e IRQ, veja na tabela a seguir suas respectivas funções.

| Pinos | Função |
|-------|--------|
| GND   | Terra  |
| VCC   | Alimentação |
| CE    | Chip Enable RX/TX |
| CSN   | SPI Chip Select |
| SCK   | SPI Clock |
| MOSI  | SPI Slave Data Input |
| MISO  | SPI Slave Data Output |
| IRQ   | Interrupção |

- Modos de operação: Modo de Power Down, de Standby, de TX e de RX, onde serão indicadas abaixo suas funções.

  > **Modo Power Down:** O Módulo Wireless é desativado usando o
consumo de corrente mínimo;

  > **Modo Standby:** Este modo é subdividido em duas categorias: Standby-I e Standby-II. O modo **Standby-I** é usado para minimizar o consumo médio de corrente enquanto mantém tempos de inicialização baixos. E o modo **Standby-II** é quando a memória extra do buffer fica ativa e é usado em comparação com o modo de Standby-I;
  
  > **Modo Transmissor:** É um modo ativo para transmissão de pacotes de dados;
  
  > **Modo Receptor:** É um modo onde o módulo wireless é usado para receber pacotes de dados;

- Tensão de alimentação: 1,9 - 3,6v;
- Corrente de alimentação: Depende de acordo com os modos de operação
programados, veja a seguir nas tabelas.

| Modo de Operação | Corrente |
|------------------|----------|
| Power Down       | 900nA    |
| StandBy-I        | 22uA     |
| StandBy-II       | 320uA    |

- A corrente média para estabelecer o Modo Transmissor é de 8mA, mas altera de acordo com a potência (dBm) programada. Veja na tabela abaixo.

| Potência (dBm) | Corrente |
|----------------|----------|
| 0              | 11,3 mA  |
| -6             | 9 mA     |
| -12            | 7,5 mA   |
| -18            | 7 mA     |
| -6 ShockBurst  | 0,12 mA  |

- A corrente média para estabelecer o Modo Receptor é de 8.4mA, porém tem pequenas variações de acordo com a velocidade de operação programada. Veja na tabela a seguir.

| Velocidade de Operação | Corrente |
|------------------------|----------|
| 1 Mbps                 | 11,8 mA  |
| 1 Mbps com LNA         | 11,1 mA  |
| 2 Mbps                 | 12,3 mA  |
| 2 Mbps com LNA         | 11,5 mA  |

- Filtro anti-interferência;
- O Módulo Wireless NRF24L01 pode atuar como emissor ou receptor, apenas realizando uma configuração por software

## Teclado Matricial 4x3
<div style="text-align: center; padding: 10px;">
  <img src="img/tecladomatricial4x3.png" width="50%" alt="Teclado Matricial 4x3">
</div>
