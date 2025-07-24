# FUNCIONAMENTO SISTEMA

### 1.2 Dispositivo do Usuário

#### Hardware
- **Conexões:**
  - MicroUSB para alimentação
  - Bluetooth 4.2 para comunicação
- **Componentes:**
  - ESP32 DevKit-V1
  - Módulo NRF24L01+

#### Aplicativo VisuTech
- **Funcionalidades:**
  - Ativação por gesto (chacoalhar)
  - Comandos por voz (API Google)
  - Geolocalização (API Google)
  - Feedback por voz (Google TTS)

### 1.2.2 Dispositivo Veicular

**Painel do Motorista:**
- Display
- Teclado matricial para seleção de linha
- Botão físico de confirmação
- Alerta sonoro (Buzzer) e visual (Display)

### 1.2.3 Fluxo de Operação

1. Usuário chacoalha o smartphone
2. App solicita comando de voz
3. Usuário fala número da linha
4. Dispositivo transmite via RF (60m)
5. Ônibus recebe sinal e alerta motorista
6. Motorista confirma parada
7. Usuário recebe confirmação por voz

<img src="img/fluxo_comunicacao_projeto.png"
     width="120%"
     style="padding: 10px">


## 2.3 Diagrama em Blocos

### 2.3.1 Funcionamentos dos Sistemas

As funcionalidades descritas abaixo referem-se ao que cada placa do sistema será capaz de realizar futuramente.

#### Sistema do ônibus (painel do motorista)

1. **Teclado Físico**:
   - Interface de entrada para o motorista
   - Permite digitar o número da linha em operação
   - Teclas dedicadas para funções especiais (confirmação, cancelamento)

2. **ESP32**:
   - Processa a entrada do teclado
   - Controla o display de informações
   - Gerencia a comunicação com o módulo RF
   - Algoritmo de verificação de dados

3. **Display**:
   - Mostra em tempo real:
     - Linha selecionada
     - Status da comunicação
     - Alertas de passageiros
   - Interface visual simples para operação em movimento

4. **Transceptor NRF24L01**:
   - Comunicação bidirecional em 2.4GHz
   - Alcance operacional: ~60m em ambiente urbano
  
5. **Sistema de Áudio:**
   - Buzzer para sinalização

#### Sistema do usuário (deficiente visual)

1. Microcontrolador ESP32
- **Função**: Processamento central do dispositivo
- **Conexões**:
  - Bluetooth com smartphone
  - Interface com módulo RF
- **Recursos**:
  - Processamento de comandos em tempo real
  - Bluetooth (BLE 4.2)

2. Transceptor NRF24L01
- **Operação**:
  - Pareamento automático com unidade veicular
  - Transmissão e recepção de dados

3. Aplicativo VisuTech
  **Interfaces**:
   - Reconhecimento de voz integrado (API Google)
   - Ativação por gestos (acelerômetro)
   
  **Funcionalidades**:
   - GPS para localização precisa
   - Sintetizador de voz (TTS) multilíngue

4. Sistema de Áudio
  - Áudio do próprio aparelho celular

<div style="text-align: center; padding: 10px;">
  <img src="img/diagrama_funcionalidades.png" width="50%" alt="Diagrama de blocos do funcionamento do circuito">
</div>

### 2.3.1 Funcionamento dos Dispositivos

As funcionalidades listadas abaixo correspondem à previsão de funcionamento do firmware. Elas serão desenvolvidas com foco nas necessidades principais do projeto, podendo ser expandidas ou ajustadas conforme novas demandas surgirem.

## Como Funciona APP VisuTech (Passo a Passo)

1. **Inicialização Inteligente**
   - Ao abrir o app, ele automaticamente:
     - Verifica conexão Bluetooth ("Conectando ao dispositivo...")
     - Anuncia: "Aparelho Conectado"
     - Identifica sua localização: "Você está no ponto:..."

2. **Solicitação de Ônibus (Simples e Intuitivo)**
   - Chacoalhe o celular para ativar
   - Fale o número da linha (exemplo: "221")
   - O app confirmia: "Procurando ônibus 221"

3. **Confirmação em Tempo Real**
   - Quando o ônibus chegar no raio de 60m:
     - "Ônibus 221 identificado"
     - "Seu ônibus está chegando"
   - Ao parar no ponto:
     - "Ônibus 221 está no ponto"

## Recursos Exclusivos

 **Controle Total por Voz**  
- Todas as ações são confirmadas por voz  
- Não precisa tocar na tela  

 **GPS Inteligente**  
- Anuncia automaticamente seu ponto de ônibus  
- Avisa em qual ponto de ônibus o usuário se encontra  

 **Alertas Sonoros**  
- Notificação quando o ônibus chegar

<div style="text-align: center; padding: 10px;">
  <img src="img/funcionamento_firmware.png" width="50%" alt="Funcionamento do Firmware">
</div>


<p align="justyfied">
  <a href="./code/visutech.md"><strong>Detalhes do código e desenvolvimento do aplicativo VisuTech</strong></a><br>
</p>

