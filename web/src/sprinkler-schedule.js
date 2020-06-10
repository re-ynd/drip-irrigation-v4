import { LitElement, html, css } from 'lit-element';

import '@polymer/paper-button/paper-button.js';
import '@polymer/paper-dropdown-menu/paper-dropdown-menu.js';
import '@polymer/paper-listbox/paper-listbox.js'
import '@polymer/paper-item/paper-item.js';

import { SprinklerWebSocket } from './sprinkler-websocket.js';

class SprinklerSchedule extends LitElement {
  static get styles() {
    return css`
      :host {
        display: flex;
        flex-direction: column;
      }

      paper-button {
        color: white;
        padding: 8px 24px;
        height: 40px;
        align-self: center;
      }

      h1 {
        font-weight: 300;
        margin: 0 -4px;
      }

      .row {
        display: flex;
        flex-direction: row;
        flex-wrap: wrap;
      }

      .margin-top {
        margin-top: 16px;
      }

      .duration {
        margin-top: -2px;
        margin-right: 32px;
        width: 200px;
      }

      .time {
        width: 200px;
        margin-right: 32px;
      }

      .default-button {
        background-color: var(--app-primary-color);
      }

      .green-button {
        background-color: var(--paper-green-700);
      }

      .red-button {
        background-color: var(--paper-red-700);
      }
    `;
  }

  constructor() {
    super();
    this._webSocket = SprinklerWebSocket.getInstance();
    this._webSocketMessageListener = this._webSocketMessageChanged.bind(this);
    this._schedules = [];
  }

  connectedCallback() {
    super.connectedCallback();
    this._webSocket.addMessageListener(this._webSocketMessageListener);
  }

  render() {
    let i = 0;
    return html`
      <h1>Schedule</h1>
      ${this._schedules.map((schedule) => this._renderSchedule(schedule, i++))}
      ${this._renderSaveAddButtons()}
    `;
  }

  _renderSchedule(schedule, id) {
    return html`
      <div class="row">
        ${this._renderTime(schedule.time, id)}
        ${this._renderDuration(schedule.duration, id)}
        <paper-button @click=${this._deleteSchedule} raised class="red-button" id=${id}>
          Delete
        </paper-button>
      </div>
    `;
  }

  _renderDuration(duration, id) {
    const content = [];
    for (let i = 1; i < 31; i++) {
      content.push(html`
        <paper-item @click=${this._durationChanged} value=${i} id=${id}>
          ${i} mins &nbsp;&nbsp;&nbsp;
        </paper-item>
      `);
    }
    return html`
      <paper-dropdown-menu label="Duration" class="duration">
        <paper-listbox slot="dropdown-content" attr-for-selected="value" selected=${duration}>
          ${content}
        </paper-listbox>
      </paper-dropdown-menu>
    `;
  }

  _renderTime(time, id) {
    return html`
      <paper-input label='Time' class="time" value=${time} type="time"  id=${id}
          @change=${this._timeChanged}>
      </paper-input>
    `;
  }

  _renderSaveAddButtons() {
    return html`
      <div class="row margin-top">
        <paper-button @click=${this._addSchedule} raised class="default-button">Add</paper-button>
        <paper-button @click=${this._saveSchedules} raised class="green-button">Save</paper-button>
      </div>
    `;
  }

  _timeChanged(e) {
    let id = parseInt(e.target.getAttribute("id"));
    this._schedules[id].time = e.target.value
  }

  _durationChanged(e) {
    let id = parseInt(e.target.getAttribute("id"));
    this._schedules[id].duration = e.target.getAttribute("value")
  }

  _deleteSchedule(e) {
    let id = parseInt(e.target.getAttribute("id"));
    this._schedules.splice(id, 1);
    super.requestUpdate();
  }

  _addSchedule(e) {
    this._schedules.push({ time: "18:00", duration: "15" });
    this.requestUpdate();
  }

  _saveSchedules(e) {
    let data = "";
    this._schedules.forEach((schedule) => {
      let tokens = schedule.time.split(":");
      let hh = parseInt(tokens[0]);
      let mm = parseInt(tokens[1]);
      let duration = parseInt(schedule.duration);
      data += ":" + hh + "|" + mm + "|" + duration;
    });
    this._webSocket.send("schedule" + data);
  }

  _webSocketMessageChanged(message) {
    let tokens = message.split(":");
    if (tokens[0] == "schedule") {
      this._schedules = [];
      tokens.slice(1).forEach((schedule) => {
        let data = schedule.split("|");
        let hh = parseInt(data[0]);
        hh = (hh < 10 ? "0" : "") + hh
        let mm = parseInt(data[1]);
        mm = (mm < 10 ? "0" : "") + mm
        let duration = data[2];
        this._schedules.push({ time: hh + ":" + mm, duration: duration});
      });
      this.requestUpdate();
    }
  }
}

customElements.define('sprinkler-schedule', SprinklerSchedule);
