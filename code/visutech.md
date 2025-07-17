# Desenvolvimento do Aplicativo

## 🛠️ Ferramentas Utilizadas

### Design de Interface
**Figma**  
- Criação do mockup inicial  
- Desenvolvimento das telas:  
  - Tela de carregamento  
  - Tela principal com componentes de voz  
  - Layout acessível com alto contraste  

![tela_inicial](https://github.com/user-attachments/assets/fc4a53b3-6aba-40b6-93c7-c8cf0dc52425)

![app](https://github.com/user-attachments/assets/2ad68767-1197-4fdf-9b40-aaee0adc3380)

### Plataforma de Desenvolvimento  
**MIT App Inventor**  
- Programação visual por blocos  
- Componentes integrados:  
  - Google Text-to-Speech API  
  - Reconhecimento de voz  
  - Sensores (acelerômetro, GPS)  
  - Comunicação Bluetooth  

## 🔧 Arquitetura Técnica

### Módulos Principais

| Componente | Função | Implementação |
|-----------|--------|---------------|
| Controle por Voz | Processa comandos do usuário | Google Speech API |
| Localização | Identifica ponto de ônibus | GPS + Geocoding |
| Comunicação | Conecta com dispositivo ESP32 | Bluetooth BLE |
| Feedback | Fornece respostas auditivas | TTS Google |

## 🧩 Blocos Lógicos Desenvolvidos

1. **Conexão Bluetooth**
   - Pairing automático com dispositivo  
   - Monitoramento contínuo da conexão  

<div style="text-align: center; padding: 10px;">
  <img src="../img/app-ble.png" width="70%" alt="bloco de conexão">
</div>

<div style="text-align: center; padding: 10px;">
  <img src="../img/app-ble2.png" width="70%" alt="bloco de conexão">
</div>

2. **Processamento de Comandos**
   - Filtro para identificar apenas números de linhas válidos  
   - Validação por expressão regular
  
<div style="text-align: center; padding: 10px;">
  <img src="../img/app-logica.png" width="70%" alt="processamento de comando">
</div>  

3. **Sistema de Localização**
   - Identificação automática do ponto
  
<div style="text-align: center; padding: 10px;">
  <img src="../img/app-localizacao.png" width="70%" alt="sistema de localização">
</div>  

4. **Temporizadores**
   - Espera por confirmação (3s)
   - Retentativas automáticas

<div style="text-align: center; padding: 10px;">
  <img src="../img/app-temporizador.png" width="70%" alt="temporizadores">
</div>  

## 📱 Tela final no celular

<div style="text-align: center; padding: 10px;">
  <img src="../img/tela-final1.jpg" width="40%" alt="tela final 1">
</div>  
<div style="text-align: center; padding: 10px;">
  <img src="../img/tela-final2.jpg" width="40%" alt="tela final 2">
</div>  
