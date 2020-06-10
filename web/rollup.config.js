// if you need to support IE11 use "modern-and-legacy-config" instead.
// import { createCompatibilityConfig } from '@open-wc/building-rollup';
// export default createCompatibilityConfig({ input: './index.html' });


import { createDefaultConfig } from '@open-wc/building-rollup';

const config = createDefaultConfig({ 
  input: './index.html',
  plugins: {
    workbox: false,  // Don't use service worker
  },
  indexHTMLPlugin: {
    polyfills: {
      hashPolyfills: false,
    }
  }
});

export default {
  ...config,
  output: {
    ...config.output,
    sourcemap: false,
    entryFileNames: '[name].js',
    chunkFileNames: '[name].js',
  },
};
