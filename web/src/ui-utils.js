import '@polymer/paper-input/paper-input.js';


export class UiUtils {

  static updateFontSizeForDropbox(el, styles) {
    if (el && el.shadowRoot) {
      const inputEl = el.shadowRoot.querySelector("paper-input");
      UiUtils.updateFontSizeForPaperInput(inputEl, styles);
    }
  }

  static updateFontSizeForPaperInput(el, styles) {
    UiUtils.updateFontSize(el, "input", styles);
    UiUtils.updateFontSize(el, "label", styles);
  }

  static updateFontSize(el, query, styles) {
    if (el && el.shadowRoot) {
      const elStyles = el.shadowRoot.querySelector(query).style;
      Object.keys(styles).map(key => elStyles[key] = styles[key]);
    }
  }
}
