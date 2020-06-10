import { LitElement, html, css } from 'lit-element';

import '@polymer/paper-button/paper-button.js';
import '@polymer/paper-dropdown-menu/paper-dropdown-menu.js';
import '@polymer/paper-listbox/paper-listbox.js'
import '@polymer/paper-item/paper-item.js';

import { UiUtils } from './ui-utils.js';
import { SprinklerWebSocket } from './sprinkler-websocket.js';

class CountdownTimer extends LitElement {
  static get styles() {
    return css`
      :host {
        --size: 270px;

        position: relative;
        height: var(--size);
        width: var(--size);
      }

      .timer {
        /* Flips the svg and makes the animation to move left-to-right */
        transform: scaleX(-1);
      }

      /* Removes SVG styling that would hide the time label */
      .timer-circle {
        fill: none;
        stroke: none;
      }

      .timer-path-remaining {
        /* Just as thick as the original ring */
        stroke-width: 7px;

        /* Rounds the line endings to create a seamless circle */
        stroke-linecap: round;

        /* Makes sure the animation starts at the top of the circle */
        transform: rotate(90deg);
        transform-origin: center;

        /* One second aligns with the speed of the countdown timer */
        transition: 1s linear all;

        /* Allows the ring to change color when the color value updates */
        stroke: currentColor;
        color: var(--app-primary-color);
      }

      /* The SVG path that displays the timer's progress */
      .timer-path-elapsed {
        stroke-width: 7px;
        stroke: var(--paper-grey-400);
      }

      .timer-center {
        position: absolute;
        
        /* Size should match the parent container */
        height: var(--size);
        width: var(--size);
        
        /* Keep the label aligned to the top */
        top: 0;
        
        /* Create a flexible box that centers content vertically and horizontally */
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
      }

      .timer-label {
        font-size: 64px;
        font-weight: 300;
        color: var(--app-text-color);
        margin-top: -12px;
      }

      .timer-dropdown {
        width: 150px;
        height: 100px;
        white-space: nowrap;
        margin-top: -16px;
      }

      paper-button {
        color: white;
        padding: 8px 24px;
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
    this._selectedDuration = 5;
    this._timeLeft = 0;
    this._duration = 1;
    this._sprinklerOn = false;
    this._webSocket = SprinklerWebSocket.getInstance();
    this._webSocket.addMessageListener(this._webSocketMessageListener.bind(this));
  }

  updated(changedProperties) {
    super.updated(changedProperties);
    this.shadowRoot.querySelectorAll("paper-dropdown-menu").forEach((element) => {
      if (element instanceof Element) {
        UiUtils.updateFontSizeForDropbox(element, { 
          fontSize: "36px", 
          fontWeight: "300",
        });
      }
    });
  }

  render() {
    return html`
      <div>
        <svg class="timer" viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg">
          <g class="timer-circle">
            <circle class="timer-path-elapsed" cx="50" cy="50" r="45" />
            <path
              id="base-timer-path-remaining"
              stroke-dasharray="283"
              class="timer-path-remaining"
              d="
                M 50, 50
                m -45, 0
                a 45,45 0 1,0 90,0
                a 45,45 0 1,0 -90,0
              "
            ></path>
          </g>
        </svg>
        <div class="timer-center">
          ${this._renderLabel()}          
          <paper-button @click=${this._toggleSprinkler} 
              class=${this._sprinklerOn ? "red-button" : "green-button"}>
            ${this._sprinklerOn ? "Turn off" : "Turn on" }
          </paper-button>
        </div>
      </div>    
    `;
  }

  _renderLabel() {
    if (this._sprinklerOn) {
      return html`<div class="timer-label">${this._renderTime(this._timeLeft)}</div>`;
    } else {
      return html`${this._renderDropdown()}`;
    }    
  }

  _renderDropdown() {
    const content = [];
    for (let i = 1; i < 31; i++) {
      content.push(html`
        <paper-item @click=${this._timeChanged} value=${i}>
          ${i} mins &nbsp;&nbsp;&nbsp;
        </paper-item>
      `);
    }
    return html`
      <paper-dropdown-menu class="timer-dropdown">
        <paper-listbox slot="dropdown-content" attr-for-selected="value"
            selected=${this._selectedDuration}>
          ${content}
        </paper-listbox>
      </paper-dropdown-menu>
    `;
  }

  _renderTime(time) {
    time = Math.round(time);
    const minutes = Math.floor(time / 60);
    let seconds = Math.round(time % 60);
    if (seconds < 10) {
      seconds = `0${seconds}`;
    }
    return html`${minutes}:${seconds}`;
  }

  _setCircleDasharray() {
    const rawTimeFraction = this._timeLeft / this._duration;
    const fraction = rawTimeFraction - (1 / this._duration) * (1 - rawTimeFraction);
    const circleDasharray = `${(
      fraction * 283
    ).toFixed(0)} 283`;
    this.shadowRoot
      .getElementById("base-timer-path-remaining")
      .setAttribute("stroke-dasharray", circleDasharray);
  }

  _timeChanged(e) {
    this._selectedDuration = parseInt(e.target.getAttribute("value"));
  }

  _webSocketMessageListener(message) {
    const tokens = message.split(':');
    if (tokens[0] === 'sprinkler') {
      this._sprinklerOn = tokens[1] === 'true';
      this._timeLeft = parseInt(tokens[2]) / 1000;
      this._duration = this._sprinklerOn ? parseInt(tokens[3]) / 1000 : 1;
      this._setCircleDasharray();
      super.requestUpdate();
    }
  }

  _toggleSprinkler(e) {
    const state = this._sprinklerOn ? 'off' : 'on';
    this._webSocket.send(`sprinkler:${state}:${this._selectedDuration * 60}`);
  }
}

customElements.define('countdown-timer', CountdownTimer);
