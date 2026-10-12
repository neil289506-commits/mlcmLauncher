// Language (zh / en) and theme (dark / light) switches. Preferences are remembered in localStorage.
(function () {
  var root = document.documentElement;
  function load(key) { try { return localStorage.getItem(key); } catch (e) { return null; } }
  function save(key, value) { try { localStorage.setItem(key, value); } catch (e) { /* storage blocked */ } }

  function setLang(lang) {
    root.setAttribute('data-lang', lang);
    root.setAttribute('lang', lang === 'zh' ? 'zh-Hant' : 'en');
    document.title = lang === 'zh' ? 'mlcmLauncher — 全新外觀的 Minecraft 啟動器' : 'mlcmLauncher — a Minecraft launcher with a fresh look';
  }
  function setTheme(theme) {
    if (theme === 'light') root.setAttribute('data-theme', 'light'); else root.removeAttribute('data-theme');
  }

  var lang = load('mlcm-lang') || ((navigator.language || '').toLowerCase().indexOf('zh') === 0 ? 'zh' : 'en');
  var theme = load('mlcm-theme') || (window.matchMedia && matchMedia('(prefers-color-scheme: light)').matches ? 'light' : 'dark');
  setLang(lang);
  setTheme(theme);

  document.addEventListener('DOMContentLoaded', function () {
    var langBtn = document.getElementById('lang-toggle');
    var themeBtn = document.getElementById('theme-toggle');
    if (langBtn) langBtn.addEventListener('click', function () {
      lang = lang === 'zh' ? 'en' : 'zh'; save('mlcm-lang', lang); setLang(lang);
    });
    if (themeBtn) themeBtn.addEventListener('click', function () {
      theme = theme === 'light' ? 'dark' : 'light'; save('mlcm-theme', theme); setTheme(theme);
    });
    document.querySelectorAll('.year').forEach(function (el) { el.textContent = new Date().getFullYear(); });
  });
})();
