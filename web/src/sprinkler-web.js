import { LitElement, html, css } from 'lit-element';

import { SprinklerWebSocket } from './sprinkler-websocket.js';
import './countdown-timer.js';
import './sprinkler-schedule.js';

class SprinklerWeb extends LitElement {
  static get properties() {
    return {
      title: { type: String },
      page: { type: String },
    };
  }

  static get styles() {
    return css`
      :host {
        --app-primary-color: var(--paper-blue-700);
        --app-text-color: #333333;

        display: flex;
        flex-direction: column;
        align-items: center;
        min-height: calc(100vh - 64px);
        max-width: 960px;
        color: var(--app-text-color);
        margin: 0 auto;
        padding: 32px;
      }

      main {
        display: flex;
        flex-direction: column;
        flex-grow: 1;
        justify-content: center;
      }

      .dashboard {
        display: flex;
        align-items: center;
        justify-content: center;
        flex-wrap: wrap;
      }

      .app-footer {
        font-size: calc(10px + 0.5vmin);
        align-items: center;
        color: #555555;
        margin-top: 32px;
      }

      paper-toggle-button {
        --paper-toggle-button-checked-bar-color: var(--app-primary-color);
        --paper-toggle-button-checked-button-color: var(--app-primary-color);
        --paper-toggle-button-checked-ink-color: var(--app-primary-color);
        --paper-toggle-button-label-color: var(--app-text-color);
      }

      .margin-bottom {
        margin-bottom: 32px;
      }

      .card {
        background-color: white;
        border-radius: 8px;
        box-shadow: 0 1px 2px 0 rgba(60,64,67,.3), 0 1px 3px 1px rgba(60,60,60,.15);
        padding: 32px;
      }

      .margin-bottom {
        margin-bottom:32px;
      }

      h1 {
        font-weight: 1;
        font-size: 64px;
        margin: 0;
        text-align: center;
      }

      .time {
        padding: 48px;
      }

      .system {
        display: flex;
      }

      .key-value {
        display: flex;
        flex-direction: column;
        padding: 0 16px;
      }

      .key {
        color: #555555;
        font-size: 0.9em;
      }

      .value {
        font-weight: 700;
      }
    `;
  }

  constructor() {
    super();
    this._webSocket = SprinklerWebSocket.getInstance();
    this._webSocketStatusListener = this._webSocketStatusChanged.bind(this);
    this._webSocketMessageListener = this._webSocketMessageChanged.bind(this);
    this._webSocketStatus = "";
    this._sprinklerOn = false;
    this._hh = "00";
    this._mm = "00";
    this._ss = "00";
    this._ram = "0";
    this._ipAddress = "0.0.0.0";
  }

  connectedCallback() {
    super.connectedCallback();
    this._webSocket.addStatusListener(this._webSocketStatusListener);
    this._webSocket.addMessageListener(this._webSocketMessageListener);
    this._webSocket.connect();
  }

  disconnectedCallback() {
    super.disconnectedCallback();
    this._webSocket.removeStatusListener(this._webSocketStatusListener);
    this._webSocket.disconnect();
  }

  render() {
    return html`
      <main>
        <div class="dashboard margin-bottom">
          <countdown-timer></countdown-timer>
          ${this._renderSystemInfo()}
        </div>
        <sprinkler-schedule class="card"></sprinkler-schedule>
      </main>
      <p class="app-footer">${this._webSocketStatus}</p>
    `;
  }

  _renderSystemInfo() {
    return html`
      <div class="time">
        <h1>${this._hh}:${this._mm}:${this._ss}</h1>
        <div class="system">
          ${this._renderKeyValue("free memory", html`${this._ram}KB`)}
          ${this._renderKeyValue("ip address", this._ipAddress)}
        </div>
      </div>
    `;
  }

  _renderKeyValue(key, value) {
    return html`
      <div class="key-value">
        <span class="value">${value}</span>
        <span class="key">${key}</span>
      </div>
    `;
  }

  _webSocketMessageChanged(message) {
    const tokens = message.split(':');
    if (tokens[0] === 'system') {
      this._hh = tokens[1];
      this._mm = tokens[2];
      this._ss = tokens[3];
      this._ram = tokens[4];
      super.requestUpdate();
    } else if (tokens[0] === 'ip_address') {
      this._ipAddress = tokens[1];
      super.requestUpdate();
    }
  }

  _webSocketStatusChanged(status) {
    this._webSocketStatus = status;
    super.requestUpdate();
  }
}

customElements.define('sprinkler-web', SprinklerWeb);
