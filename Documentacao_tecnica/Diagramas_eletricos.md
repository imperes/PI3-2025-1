# DIAGRÂMAS ELÉTRICOS
---
## Placa Usuário 

### Esquemático
O hardware do usuário foi desenvolvido com base na placa legada do projeto anterior. Nesta nova versão, a alimentação do módulo NRF24L01 é fornecida diretamente pelo pino 3V3 da placa ESP32-H2-DevKitM-1, eliminando a necessidade de um circuito externo para conversão da tensão de 5V proveniente da porta USB para 3,3V.

Adicionalmente, foi inserido um capacitor de 100 nF entre o pino 3V3 e o GND para filtragem de ruídos. Esse capacitor pode ser substituído por outro de maior capacitância, caso haja necessidade de maior rejeição de ruídos que possam ser conduzidos entre as placas através da linha de alimentação.

A interface de comunicação entre o módulo NRF24L01 e o ESP32 foi configurada com os seguintes pinos:


| Pinos | Função                | Pino no ESP32-H2-DevKitM-1         |
|-------|-----------------------|------------------------------------|
| GND   | Terra                 | GND                                |
| VCC   | Alimentação           | 3V3                                |
| CE    | Chip Enable RX/TX     | GPIO13                             |
| CSN   | SPI Chip Select       | FSPICS0                            |
| SCK   | SPI Clock             | FSPICLK                            |
| MOSI  | SPI Slave Data Input  | FSPID                              |
| MISO  | SPI Slave Data Output | FSPIQ                              |
| IRQ   | Interrupção           | GPIO8                              |



O esquemático elétrico foi desenvolvido utilizando o software Altium Designer e está representado conforme a imagem abaixo:

## Esquemático Elétrico - Motorista
<div style="text-align: center; padding: 10px;">
  <img src="img../esquematico-motorista.jpg" width="40%" alt="Esquemático Elétrico Motorista">
</div>

## Esquemático Elétrico - Receptor
<div style="text-align: center; padding: 10px;">
  <img src="img../esquematico_pcb_receptor.PNG" width="40%" alt="Esquemático Elétrico Receptor">
</div>

## Esquemático Elétrico - Usuário
<div style="text-align: center; padding: 10px;">
  <img src="img../esquematico_pcb_usuario.PNG" width="40%" alt="Esquemático Elétrico Usuário">
</div>

Com base no esquemático, foi desenvolvida uma placa de circuito impresso (PCI) no Altium Designer. As trilhas e pads foram desenhados com larguras maiores para facilitar a fabricação manual, especialmente no processo de transferência térmica com papel glossy. Isso melhora a transferência da tinta para a placa de cobre e reduz falhas durante a corrosão com percloreto de ferro.

## PCB - Motorista
<div style="text-align: center; padding: 10px;">
  <img src="img../Pcb-motorista.jpg" width="40%" alt="PCB Motorista">
</div>

## PCB - Usuário
<div style="text-align: center; padding: 10px;">
  <img src="img../Pcb-usuario.jpg" width="40%" alt="PCB Usuário">
</div>

--- 
## Placa Motorista

O desenvolvimento do esquemático da placa do motorista considerou, em primeiro lugar, a compatibilidade com a comunicação utilizada na placa do usuário, baseada no módulo NRF24L01. Para isso, foram utilizados os pinos SPI da placa ESP32-DevKit V1, conectando o NRF24L01 de forma direta e eficiente.

Além da comunicação sem fio, foi necessário incluir um display para exibir ao motorista a linha digitada pelo motorista. Esse display também foi conectado via SPI, aproveitando os pinos de comunicação da ESP32, e alimentado pelos pinos VCC e GND da própria placa.

Outro componente essencial foi o teclado matricial 4x4, utilizado para entrada local de dados. Para sua conexão, foram utilizados 8 pinos GPIO com resistores de pull-up internos, o que eliminou a necessidade de resistores externos, contribuindo para a redução do tamanho da PCB e simplificação do circuito.

Por fim, foi adicionado um buzzer com função de alerta sonoro, ativado sempre que uma parada for solicitada. Esse componente foi conectado a um pino GPIO com suporte a PWM, permitindo o controle de intensidade e padrão do som emitido.


## Placa - Motorista
<div style="text-align: center; padding: 10px;">
  <img src="img../placa-motorista.jpg" width="40%" alt="Placa Motorista">
</div>

## Placa - Usuário
<div style="text-align: center; padding: 10px;">
  <img src="img../placa-usuario.jpg" width="40%" alt="Placa Usuário">
</div>

