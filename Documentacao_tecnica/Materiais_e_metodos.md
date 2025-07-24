# 3. MATERIAIS E MÉTODOS

## Display OLED
<div style="text-align: center; padding: 10px;">
  <img src="img/display.jpg" width="40%" alt="Display OLED">
</div>

## Buzzer
<div style="text-align: center; padding: 10px;">
  <img src="img/buzzer.png" width="30%" alt="Buzzer">
</div>

## Módulo Wireless NRF24L01
<div style="text-align: center; padding: 10px;">
  <img src="img/nrf24l01.png" width="30%" alt="NRF24L01">
</div>

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

## Teclado Matricial 4x4
<div style="text-align: center; padding: 10px;">
  <img src="img/teclado_matricial.jpg" width="30%" alt="Teclado Matricial 4x4">
</div>


## Microcontrolador ESP32 DevKit-v1
<div style="text-align: center; padding: 10px;">
  <img src="img/esp32-devkit-v1.jpg" width="30%" alt="ESP 32">
</div>

### Características Técnicas da ESP32 DevKit V1

#### Processamento
- **Frequência de operação**: Dual-core a 160MHz ou single-core a 240MHz (ajustável)
- **Velocidade de processamento**: ~4.17ns por ciclo (240MHz)
- **Arquitetura**: Xtensa LX6 (32-bit)

#### GPIO e Pinagem
- **Total de pinos**: 38 (na versão DevKit V1)
- **GPIOs disponíveis**: 25 (com restrições de uso)
  - Entradas analógicas: 18 canais ADC de 12-bit
  - Saídas analógicas: 8 canais DAC de 8-bit
- **Pinos especiais**:
  - 16 pinos com touch capacitivo
  - 4 pinos SPI dedicados
  - 2 pinos I2C

#### Consumo Energético
- **Modos de operação**:
  - Ativo: ~160mA (máx.)
  - Modem-sleep: ~20mA
  - Light-sleep: ~0.4mA
  - Deep-sleep: ~10µA

#### Memórias
- **Flash integrada**: 4MB (ESP-WROOM-32)
- **SRAM**: 520KB (sendo 320KB para dados e 200KB para instruções)
- **RTC Fast Memory**: 8KB (para deep-sleep)
- **EEPROM**: Emulada na flash (até 512KB)

#### Alimentação
- **Tensão operacional**: 2.2V a 3.6V
- **Tensão recomendada**: 3.3V
- **Consumo típico**: 80mA em operação normal

#### Conectividade
- **Wi-Fi**:
  - 802.11 b/g/n
  - 2.4GHz
  - Até 150Mbps
- **Bluetooth**:
  - Bluetooth 4.2 BR/EDR
  - BLE

#### Periféricos
- **Interfaces seriais**:
  - 3× UART
  - 2× I2C
  - 2× I2S
  - 3× SPI (1 dedicado para flash)
  
- **Outros periféricos**:
  - 16× PWM LED
  - 2× DAC
  - 10× sensores touch capacitivos
  - 2× timers de watchdog
  - 4× timers de uso geral
  - Interface Ethernet MAC (requer PHY externo)

#### Características Físicas
- **Dimensões PCB**: ~52mm × 28mm
- **Interface de programação**: Micro USB (CP2102)
- **Botões**:
  - Reset
  - Boot (para modo flash)

## 3.1 Orçamentos
### 3.1.1 Orçamento do primeiro protótipo (2017)
O orçamento abaixo foi feito em 2017 e a compra dos materiais foram em lojas físicas, o que deixa mais caro os componentes, então esse orçamento é apenas uma base pois comprando online em fornecedores e em escala os valores são ainda menores
#### Orçamento do protótipo do transmissor 2017 (usuário/deficiente visual)

<div style="text-align: center; padding: 10px;">
  <img src="img/orcamento_tx_antigo.png" width="50%" alt="Orçamento do protótipo do transmissor em 2017">
</div>

#### Orçamento do protótipo do receptor 2017 (veículo/ônibus)

<div style="text-align: center; padding: 10px;">
  <img src="img/orcamento_rx_antigo.png" width="50%" alt="Orçamento do protótipo do receptor em 2017">
</div>

#### Orçamento total 2017

<div style="text-align: center; padding: 10px;">
  <img src="img/orcamento_total_antigo.png" width="30%" alt="Orçamento do protótipo total em 2017">
</div>

### 3.1.2 Orçamento do novo protótipo (2025)
O orçamento desenvolvido foi com base no antigo, levando em conta que será dado foco apenas no transmissor para fazer o upgrade e, por isso, não será mexido no receptor. Como não é certo a quantidade de componentes como resistores, capacitores, cabos, entre outros componentes, o custo pode variar tanto para mais, quanto para menos. O preço dos componentes internacionais já foram adicionados as taxas e impostos.
#### Orçamento do protótipo do transmissor 2025 (usuário/deficiente visual)

<div style="text-align: center; padding: 10px;">
  <img src="img/orcamento_tx_novo_v2.png" width="50%" alt="Orçamento do protótipo do transmissor em 2025">
</div>

#### Orçamento total 2017/2025

<div style="text-align: center; padding: 10px;">
  <img src="img/orcamento_total_novo.png" width="30%" alt="Orçamento do protótipo total em 2025">
</div>

### 3.3 Vantagens da Nova Versão e Arquitetura do Sistema Atualizado

✔ **Autonomia:**  
- Funciona sem internet (apenas RF e BLueTooth)    

✔ **Acessibilidade:**  
- Controle por voz e gestos  
- Feedback por áudio, visual e vibração

O projeto original recebeu significativas melhorias tecnológicas:

**Nova Configuração:**
- **Microcontrolador:** ESP32 DevKit-V1 (substituindo o PIC16F877A)
- **Módulo RF:** NRF24L01+ com alcance de ~60m
- **Interface:** Aplicativo VisuTech + dispositivo

## 3.4 Placas de circuito Impresso

### 3.4.1 Hardware do Usuário

Também foi tomado o cuidado de manter as dimensões da placa reduzidas, garantindo que ela seja compacta o suficiente para ser instalada na parte traseira de um gabinete acoplado ao celular.

<div style="text-align: center; padding: 10px;">
  <img src="img/PCB.png" width="50%" alt="PCB">
</div>

Também foram adicionados os modelos 3D dos componentes e da placa no projeto do Altium Designer, permitindo uma visualização mais realista do dispositivo final. Isso facilita a verificação do encaixe mecânico e auxilia no planejamento do posicionamento dentro do gabinete.

<div style="text-align: center; padding: 10px;">
  <img src="img/PCB_3D_TOP.png" width="50%" alt="PCB">
</div>

Foi realizado no software Proteus o esquemático da PCB do Receptor (Motorista), demonstrado na imagem abaixo:
<div style="text-align: center; padding: 10px;">
  <img src="img/esquematico_pcb_receptor.PNG" width="50%" alt="PCB">
</div>

### 3.4.2 Hardware Desenvolvido
**Transmissor (controle do deficiente)**

<div style="text-align: center; padding: 10px;">
  <img src="./img/Transmissor (controle do deficiente).jpeg" width="40%" alt="Parte Interna (circuito - motorista)">
</div>

**Parte Interna (circuito)**

<div style="text-align: center; padding: 10px;">
  <img src="./img/Parte Interna (circuito).jpeg" width="40%" alt="Parte Interna (circuito - motorista)">
</div>

**Receptor (Motorista)**

<div style="text-align: center; padding: 10px;">
  <img src="./img/Receptor (Motorista).jpeg" width="40%" alt="Parte Interna (circuito - motorista)">
</div>

**Parte Interna (circuito - motorista)**

<div style="text-align: center; padding: 10px;">
  <img src="./img/Parte Interna (circuito - motorista).jpeg" width="40%" alt="Parte Interna (circuito - motorista)">
</div>

### 3.4.3 Aplicativo VisuTech (Usuário) 
![tela_inicial](https://github.com/user-attachments/assets/fc4a53b3-6aba-40b6-93c7-c8cf0dc52425)

VisuTech – Mobilidade e Autonomia para Pessoas com Deficiência Visual
> VisuTech — Tecnologia que guia, conecta e transforma vidas.

O **VisuTech** é um aplicativo pioneiro desenvolvido exclusivamente para pessoas com deficiência visual, transformando seu smartphone em um assistente pessoal para uso do transporte público com total autonomia.


![app](https://github.com/user-attachments/assets/2ad68767-1197-4fdf-9b40-aaee0adc3380)


![image](https://github.com/user-attachments/assets/2ab5d090-6b4c-4196-bac7-ea4f96eebe20)


<p align="justyfied">
  <a href="./code/visutech.md"><strong>Detalhes do código e desenvolvimento do aplicativo VisuTech</strong></a><br>
</p>


---

<p align="justyfied">
  <a href="./README.md"><strong>RESUMO</strong></a><br>
  <a href="./Introducao.md"><strong>1. INTRODUÇÃO</strong></a><br>
  <a href="./Referencial_teorico.md"><strong>2. REFERENCIAL TEÓRICO</strong></a><br>
  <a href="./Conclusao.md"><strong>4. CONCLUSÃO</strong></a><br>
  <a href="./Referencias_bibliograficas.md"><strong>5. REFERÊNCIAS BIBLIOGRÁFICAS</strong></a>
</p>
