(function () {
  'use strict';
  const data = window.CTASK_DATA;
  const main = document.getElementById('main-content');
  if (!data || !window.CTaskLabs) {
    main.innerHTML = '<p class="loading-fallback">课程文件未能加载。请保留整个 dist 文件夹，再打开 index.html。</p>';
    return;
  }
  const icons = {
    grid:'<rect x="3" y="3" width="7" height="7" rx="1.5"/><rect x="14" y="3" width="7" height="7" rx="1.5"/><rect x="3" y="14" width="7" height="7" rx="1.5"/><rect x="14" y="14" width="7" height="7" rx="1.5"/>',
    book:'<path d="M12 6c-3-2-6-2-9-1v15c3-1 6-1 9 1 3-2 6-2 9-1V5c-3-1-6-1-9 1Zm0 0v15"/>',
    lab:'<path d="M9 3h6m-5 0v7L4 19a1 1 0 0 0 1 2h14a1 1 0 0 0 1-2l-6-9V3M7 15h10"/><path d="M10 17h.01M14 18h.01"/>',
    terminal:'<rect x="3" y="4" width="18" height="16" rx="2"/><path d="m7 9 3 3-3 3m6 0h4"/>',
    code:'<path d="m8 7-5 5 5 5m8-10 5 5-5 5m-3-13-2 20"/>',
    folder:'<path d="M3 7V5a2 2 0 0 1 2-2h5l3 3h6a2 2 0 0 1 2 2v11a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2Z"/>',
    route:'<circle cx="6" cy="5" r="2"/><circle cx="18" cy="19" r="2"/><path d="M6 7v7a4 4 0 0 0 4 4h6M8 5h6a4 4 0 0 1 0 8h-4"/>',
    search:'<circle cx="10.5" cy="10.5" r="6.5"/><path d="m16 16 5 5"/>',
    arrow:'<path d="M4 12h16m-6-6 6 6-6 6"/>',
    chevron:'<path d="m9 5 7 7-7 7"/>',
    check:'<path d="m5 12 4 4L19 6"/>',
    clock:'<circle cx="12" cy="12" r="9"/><path d="M12 7v5l3 2"/>',
    download:'<path d="M12 3v12m-5-5 5 5 5-5M4 15v5h16v-5"/>',
    menu:'<path d="M4 6h16M4 12h16M4 18h16"/>',
    pointer:'<path d="m4 3 6 18 3-8 8-3L4 3Z"/>',
    memory:'<rect x="5" y="5" width="14" height="14" rx="2"/><path d="M9 1v4m6-4v4M9 19v4m6-4v4M1 9h4m-4 6h4m14-6h4m-4 6h4"/>',
    flag:'<path d="M5 22V3c5-4 9 4 14 0v11c-5 4-9-4-14 0"/>',
    text:'<path d="M4 5h16M12 5v15M8 20h8M4 9V5m16 4V5"/>',
    star:'<path d="m12 3 2.8 5.7 6.2.9-4.5 4.4 1.1 6.2-5.6-3-5.6 3 1.1-6.2L3 9.6l6.2-.9L12 3Z"/>',
    trash:'<path d="M3 6h18M9 6V3h6v3M5 6l1 15h12l1-15M10 10v7m4-7v7"/>',
    info:'<circle cx="12" cy="12" r="9"/><path d="M12 11v6m0-10v1"/>',
    reset:'<path d="M3 10a9 9 0 1 1 2 8M3 4v6h6"/>',
    copy:'<rect x="8" y="8" width="13" height="13" rx="2"/><path d="M16 8V3H3v13h5"/>'
  };
  function icon(name) { return '<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.6" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">' + (icons[name] || icons.code) + '</svg>'; }
  function paintIcons(root = document) { root.querySelectorAll('[data-icon]').forEach(el => { el.innerHTML = icon(el.dataset.icon); }); }
  function esc(value) { return String(value).replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c])); }
  const storageKey = 'ctask-visual-progress-v1';
  let storageAvailable = true;
  function load(key, fallback) {
    try { const value = localStorage.getItem(key); return value ? JSON.parse(value) : fallback; }
    catch (_) { storageAvailable = false; return fallback; }
  }
  const rawStored = load(storageKey, {});
  const stored = rawStored && typeof rawStored === 'object' ? rawStored : {};
  const state = {
    completed: Array.isArray(stored.completed) ? stored.completed.filter(x => Number.isInteger(x) && x >= 1 && x <= 22) : [],
    passed: stored.passed && typeof stored.passed === 'object' ? stored.passed : {},
    bookmarks: Array.isArray(stored.bookmarks) ? stored.bookmarks : [],
    drafts: stored.drafts && typeof stored.drafts === 'object' ? stored.drafts : {},
    lastChapter: Number.isInteger(stored.lastChapter) ? stored.lastChapter : 1
  };
  let taskData = load('ctask-visual-tasks-v1', null);
  if (!taskData || !Array.isArray(taskData.items) || !Number.isSafeInteger(taskData.next_id) || taskData.next_id < 1) {
    taskData = {items:[{id:1,title:'运行第一个 C 程序',priority:3,flags:0},{id:2,title:'画出变量与指针的关系',priority:2,flags:2},{id:3,title:'完成温度转换练习',priority:2,flags:0}],next_id:4,cap:8};
  }
  let taskFilter = 'all';
  let lastOperation = '示例数据已准备，或已从当前浏览器恢复。数据在网页中模拟，C 实现见源码。';
  let toastTimer;
  function toast(message) {
    const el = document.getElementById('toast');
    el.textContent = message; el.hidden = false; clearTimeout(toastTimer);
    toastTimer = setTimeout(() => { el.hidden = true; }, 2600);
  }
  function persist(key, value) {
    try { localStorage.setItem(key, JSON.stringify(value)); }
    catch (_) { storageAvailable = false; }
    updateProgress();
  }
  function save() { persist(storageKey, state); }
  function updateProgress() {
    document.getElementById('side-progress').textContent = state.completed.length + ' / 22';
    document.getElementById('side-progress-bar').style.width = state.completed.length / 22 * 100 + '%';
    document.getElementById('storage-status').textContent = storageAvailable ? '进度保存在当前浏览器' : '浏览器限制存储，进度暂存在本次会话';
  }
  async function copy(text) {
    try {
      if (navigator.clipboard && window.isSecureContext) { await navigator.clipboard.writeText(text); }
      else {
        const field = document.createElement('textarea'); field.value = text; field.style.position = 'fixed'; field.style.top = '-1000px'; document.body.append(field); field.select();
        const copied = document.execCommand('copy'); field.remove(); if (!copied) throw new Error('clipboard unavailable');
      }
      toast('已复制');
    } catch (_) { toast('浏览器不允许自动复制，请选中文字后复制。'); }
  }
  function download(name, text, type = 'text/plain;charset=utf-8') {
    const url = URL.createObjectURL(new Blob([text], {type}));
    const link = document.createElement('a'); link.href = url; link.download = name; link.click(); setTimeout(() => URL.revokeObjectURL(url), 1000);
  }
  function syntax(code) {
    const tokens = /(\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|^[ \t]*#[^\n]*|\b(?:int|char|void|const|static|return|if|else|for|while|do|switch|case|break|continue|struct|union|enum|typedef|unsigned|long|short|double|float|sizeof|bool|true|false|NULL|inline|restrict|volatile|extern|_Generic|_Static_assert)\b|\b\d+(?:\.\d+)?(?:u|l|f)*\b)/gm;
    let result = '', last = 0;
    for (const match of code.matchAll(tokens)) {
      result += esc(code.slice(last, match.index));
      const value = match[0];
      const cls = /^\s*#/.test(value) ? 'preprocessor' : /^\//.test(value) ? 'comment' : /^["']/.test(value) ? 'string' : /^\d/.test(value) ? 'number' : 'keyword';
      result += '<span class="token-' + cls + '">' + esc(value) + '</span>'; last = match.index + value.length;
    }
    return result + esc(code.slice(last));
  }
  function codePanel(code, label = 'C · C17') {
    return '<div class="code-panel"><div class="code-header"><span>' + esc(label) + '</span><button type="button" data-copy-code>复制代码</button></div><pre><code class="code-content">' + syntax(code) + '</code></pre></div>';
  }
  function markdownLink(href) {
    if (/^https?:\/\//i.test(href)) return {href, external:true};
    if (/COURSE\.md/i.test(href)) return {href:'#/course/1'};
    if (/START_HERE\.md/i.test(href)) return {href:'#/doc/start'};
    if (/PROJECT_GUIDE\.md/i.test(href)) return {href:'#/doc/guide'};
    if (/ROADMAP\.md/i.test(href)) return {href:'#/doc/roadmap'};
    if (/GLOSSARY\.md/i.test(href)) return {href:'#/doc/glossary'};
    if (/stages\/README\.md/i.test(href)) return {href:'#/doc/stages'};
    if (/lessons\/README\.md/i.test(href)) return {href:'#/doc/lessons'};
    if (/exercises\/README\.md/i.test(href)) return {href:'#/doc/exercises'};
    if (/README\.md/i.test(href)) return {href:'#/doc/readme'};
    if (/exercises/i.test(href)) return {href:'#/exercises/01_temperature'};
    return {href:'#/source'};
  }
  function inline(text) {
    const fragments = [];
    let value = text.replace(/`([^`]+)`/g, (_, code) => '\u0000' + (fragments.push('<code>' + esc(code) + '</code>') - 1) + '\u0000');
    value = value.replace(/\[([^\]]+)\]\(([^)]+)\)/g, (_, label, href) => {
      const link = markdownLink(href);
      return '\u0000' + (fragments.push('<a href="' + esc(link.href) + '"' + (link.external ? ' target="_blank" rel="noopener noreferrer"' : '') + '>' + esc(label) + '</a>') - 1) + '\u0000';
    });
    return esc(value).replace(/\*\*([^*]+)\*\*/g, '<strong>$1</strong>').replace(/\u0000(\d+)\u0000/g, (_, index) => fragments[Number(index)]);
  }
  function cells(line) {
    const parts = []; let text = '', inCode = false;
    for (const ch of line.replace(/^\s*\|/, '').replace(/\|\s*$/, '')) {
      if (ch === '`') inCode = !inCode;
      if (ch === '|' && !inCode) { parts.push(text.trim()); text = ''; } else { text += ch; }
    }
    parts.push(text.trim()); return parts;
  }
  function markdown(text) {
    const lines = text.replace(/\r/g, '').split('\n'); const output = []; let i = 0;
    while (i < lines.length) {
      const line = lines[i];
      if (!line.trim()) { i++; continue; }
      if (/^```/.test(line)) {
        const lang = line.slice(3).trim() || 'text'; const code = []; i++;
        while (i < lines.length && !/^```/.test(lines[i])) code.push(lines[i++]);
        if (i < lines.length) i++;
        output.push(codePanel(code.join('\n'), lang === 'c' ? 'C · C17' : lang)); continue;
      }
      const heading = line.match(/^(#{1,6})\s+(.+)$/);
      if (heading) { const level = Math.min(4, heading[1].length + 1); output.push('<h' + level + '>' + inline(heading[2]) + '</h' + level + '>'); i++; continue; }
      if (line.includes('|') && i + 1 < lines.length && /^\s*\|?\s*:?-{3,}/.test(lines[i + 1])) {
        const head = cells(line); i += 2; const rows = [];
        while (i < lines.length && lines[i].includes('|') && lines[i].trim()) rows.push(cells(lines[i++]));
        output.push('<div class="table-scroll"><table><thead><tr>' + head.map(x=>'<th>'+inline(x)+'</th>').join('') + '</tr></thead><tbody>' + rows.map(row=>'<tr>'+row.map(x=>'<td>'+inline(x)+'</td>').join('')+'</tr>').join('') + '</tbody></table></div>'); continue;
      }
      if (/^\s*([-*_])\1\1+\s*$/.test(line)) { output.push('<hr>'); i++; continue; }
      if (/^\s*(?:[-*]|\d+\.)\s+/.test(line)) {
        const ordered = /^\s*\d+\./.test(line), tag = ordered ? 'ol' : 'ul', rows = [];
        while (i < lines.length && /^\s*(?:[-*]|\d+\.)\s+/.test(lines[i])) rows.push('<li>' + inline(lines[i++].replace(/^\s*(?:[-*]|\d+\.)\s+/, '')) + '</li>');
        output.push('<' + tag + '>' + rows.join('') + '</' + tag + '>'); continue;
      }
      if (/^>\s?/.test(line)) { output.push('<blockquote>' + inline(line.replace(/^>\s?/, '')) + '</blockquote>'); i++; continue; }
      if (/^<\/?details>\s*$/.test(line.trim())) { output.push(line.trim()); i++; continue; }
      const summary = line.match(/^<summary>(.*?)<\/summary>$/);
      if (summary) { output.push('<summary>' + esc(summary[1]) + '</summary>'); i++; continue; }
      const paragraph = [line]; i++;
      while (i < lines.length && lines[i].trim() && !/^(?:#{1,6}\s|```|\s*[-*]\s|\s*\d+\.\s|\|)/.test(lines[i])) paragraph.push(lines[i++]);
      output.push('<p>' + inline(paragraph.join(' ')) + '</p>');
    }
    return output.join('');
  }
  function heading(title, description, action = '') {
    return '<div class="page-heading"><div><div class="eyebrow">C LEARNING STUDIO</div><h1>' + title + '</h1><p>' + description + '</p></div>' + action + '</div>';
  }
  function overview() {
    const next = data.chapters.find(c => !state.completed.includes(c.id)) || data.chapters[0];
    const paths = [
      ['01','从第一行代码开始','程序结构 · 类型 · 分支与循环','1—4',1],
      ['02','让数据有自己的位置','数组 · 字符串 · 指针 · 结构体','5—9',5],
      ['03','把任务管理器做完整','动态内存 · 多文件 · 业务接口','10—12',10],
      ['04','让数据留下来','文件存储 · 算法 · 容器','13—16',13],
      ['05','从会写走向写得可靠','预处理 · 标准库 · 调试与移植','17—20',17],
      ['06','独立完成你的改造','现代 C · 并发选学 · 毕业项目','21—22',21]
    ];
    main.innerHTML = heading('今天，也让代码前进一步。','从一个任务管理器出发，把 C 语言一点点学扎实。','<span class="quiet-label">'+icon('route')+' 按自己的节奏学习</span>') +
    '<section class="hero"><div><div class="eyebrow">ONE PROJECT. THE WHOLE JOURNEY.</div><h2>从第一行代码，<br>走到完整项目。</h2><p>写下一个变量，观察一次内存变化，再把知识用进真实项目。<br>每个概念，都有一个看得见的下一步。</p><div class="hero-actions"><a class="btn btn-primary" href="#/course/'+next.id+'">'+(state.completed.length?'继续学习':'开始第一章')+icon('arrow')+'</a><a class="text-link" href="#/doc/start">先看新手指南 '+icon('chevron')+'</a></div></div><div class="hero-art" aria-label="C代码任务卡片示意"><div class="hero-code"><div class="window-dots"><i></i><i></i><i></i><span>my_first_task.c</span></div><pre><span class="purple">#include</span> &lt;stdio.h&gt;\n\n<span class="purple">int</span> main(<span class="purple">void</span>) {\n    <span class="purple">int</span> progress = <span class="yellow">0</span>;\n    progress += <span class="yellow">1</span>;\n    printf(<span class="green">"Every step counts.\\n"</span>);\n    <span class="purple">return</span> <span class="yellow">0</span>;\n}</pre></div><div class="hero-note">'+icon('check')+'<div><strong>你的第一个程序，从这里开始</strong>运行 → 观察 → 修改 → 理解</div></div></div></section>' +
    '<div class="stats-row">'+[['book','22','章','循序渐进的中文课程'],['lab','16','个','独立可运行的 C 实验'],['code','6','道','带参考答案的动手练习'],['check',String(state.completed.length),' / 22','已完成的章节']].map(s=>'<div class="surface stat"><div class="stat-icon">'+icon(s[0])+'</div><div><strong>'+s[1]+'<small>'+s[2]+'</small></strong><p>'+s[3]+'</p></div></div>').join('')+'</div>'+
    '<div class="dashboard-columns"><section><div class="section-header"><h2>你的学习路线</h2><a class="text-link" href="#/doc/roadmap">完整知识地图 '+icon('arrow')+'</a></div><div class="surface path-list">'+paths.map(p=>'<a class="path-row" href="#/course/'+p[4]+'"><span class="path-number">'+p[0]+'</span><div class="path-copy"><strong>'+p[1]+'</strong><small>'+p[2]+'</small></div><span class="pill">第 '+p[3]+' 章</span>'+icon('chevron')+'</a>').join('')+'</div></section><section><div class="section-header"><h2>把抽象知识，看明白</h2><span class="pill pill-green">可交互</span></div><div class="surface lab-teaser"><span class="pill pill-purple">指针与内存</span><h3>变量是盒子，<br>指针记着盒子的地址。</h3><p>点击“下一步”，亲眼看到 <code>*p = 35</code> 如何改变原变量。</p><div class="mini-memory" aria-hidden="true"><div class="memory-box"><small>pointer p</small>0x100</div><span>→</span><div class="memory-box"><small>score</small>35</div></div><a class="text-link" href="#/lab/pointer">进入可视化实验 '+icon('arrow')+'</a></div><a href="#/project" class="surface quick-lesson"><span class="stat-icon">'+icon('terminal')+'</span><div><h3>先体验一下，你将做出的项目</h3><p>添加任务 · 切换状态 · 观察数据变化</p></div>'+icon('chevron')+'</a><a href="#/doc/guide" class="surface quick-lesson"><span class="stat-icon">'+icon('folder')+'</span><div><h3>一条任务，怎样穿过整个程序？</h3><p>从菜单输入，追踪到内存与文件</p></div>'+icon('chevron')+'</a></section></div>';
  }
  function renderCourse(id) {
    const chapter = data.chapters.find(c => c.id === Number(id)) || data.chapters[0];
    state.lastChapter = chapter.id; save();
    const passed = !!state.passed[chapter.id], completed = state.completed.includes(chapter.id), marked = state.bookmarks.includes(chapter.id);
    main.innerHTML = '<div class="course-grid"><aside class="surface chapter-list" aria-label="章节目录"><div class="chapter-list-header"><strong>课程目录</strong><small>22 个章节</small></div>'+data.chapters.map(c=>'<a class="chapter-item '+(c.id===chapter.id?'active':'')+'" href="#/course/'+c.id+'" '+(c.id===chapter.id?'aria-current="page"':'')+'><span class="chapter-num">'+String(c.id).padStart(2,'0')+'</span><span class="chapter-title">'+esc(c.title)+'</span>'+(state.completed.includes(c.id)?'<span class="chapter-check" aria-label="已完成">✓</span>':'')+'</a>').join('')+'</aside><div class="course-body"><article class="surface chapter-article"><div class="chapter-topline"><span class="eyebrow">CHAPTER '+String(chapter.id).padStart(2,'0')+' / 22</span><button id="bookmark-chapter" class="icon-button '+(marked?'bookmark-on':'')+'" aria-label="'+(marked?'取消收藏本章':'收藏本章')+'" aria-pressed="'+marked+'">'+icon('star')+'</button></div><h1>'+esc(chapter.title)+'</h1><div class="article-meta"><span class="pill pill-purple">'+esc(chapter.level)+'</span><span>先预测，再运行，最后亲手修改</span></div><div class="markdown">'+markdown(chapter.markdown)+'</div></article><section class="surface quiz-card" aria-label="本章自测"><div class="eyebrow">CHECK YOUR UNDERSTANDING</div><h2>停一下，检查是否理解</h2><p>'+esc(chapter.quiz.question)+'</p><form id="chapter-quiz"><div class="quiz-options">'+chapter.quiz.options.map((option,i)=>'<label class="quiz-option"><input type="radio" name="answer" value="'+i+'" required><span>'+String.fromCharCode(65+i)+'. '+esc(option)+'</span></label>').join('')+'</div><div class="quiz-submit-row"><button class="btn btn-primary" type="submit">检查答案 '+icon('check')+'</button><small>'+(passed?'本章自测已通过，仍可再次作答。':'通过自测后，可以标记本章完成。')+'</small></div><div id="quiz-feedback" role="status" hidden></div></form></section><div class="chapter-bottom"><button id="complete-chapter" class="btn '+(completed?'':'btn-primary')+'" '+(!passed?'disabled':'')+'>'+icon('check')+(completed?'已完成 · 点击撤销':'标记本章完成')+'</button><div>'+(chapter.id>1?'<a class="btn" href="#/course/'+(chapter.id-1)+'">上一章</a>':'')+(chapter.id<22?'<a class="btn" href="#/course/'+(chapter.id+1)+'">下一章 '+icon('arrow')+'</a>':'<a class="btn" href="#/project">进入项目实战 '+icon('arrow')+'</a>')+'</div></div></div></div>';
    document.getElementById('bookmark-chapter').addEventListener('click', ev => {
      const index = state.bookmarks.indexOf(chapter.id); if (index === -1) state.bookmarks.push(chapter.id); else state.bookmarks.splice(index,1);
      const selected = state.bookmarks.includes(chapter.id); ev.currentTarget.classList.toggle('bookmark-on', selected); ev.currentTarget.setAttribute('aria-pressed', String(selected)); ev.currentTarget.setAttribute('aria-label', selected?'取消收藏本章':'收藏本章'); save(); toast(selected?'已收藏本章':'已取消收藏');
    });
    document.getElementById('chapter-quiz').addEventListener('submit', ev => {
      ev.preventDefault(); const selected = main.querySelector('input[name="answer"]:checked'); if (!selected) return;
      const correct = Number(selected.value) === chapter.quiz.answer; const feedback = document.getElementById('quiz-feedback'); feedback.hidden = false; feedback.className = 'quiz-feedback'+(correct?' success':'');
      feedback.textContent = (correct?'回答正确。':'还差一点。') + chapter.quiz.explanation;
      if (correct) { state.passed[chapter.id] = true; save(); document.getElementById('complete-chapter').disabled = false; }
    });
    document.getElementById('complete-chapter').addEventListener('click', () => {
      if (!state.passed[chapter.id]) return;
      const at = state.completed.indexOf(chapter.id); if (at === -1) state.completed.push(chapter.id); else state.completed.splice(at,1);
      save(); renderCourse(chapter.id); toast(at === -1?'本章已完成，继续下一步。':'已撤销完成状态');
    });
  }
  function renderLab(type) {
    const labs = [['pointer','pointer','指针与变量'],['memory','memory','动态数组与扩容'],['bits','flag','状态位与位运算'],['strings','text','字符串与 UTF-8']];
    if (!labs.some(x=>x[0]===type)) type='pointer';
    main.innerHTML = heading('把内存里的变化，看得见。','一步一步观察对象、地址和字节，再回到 C 源码验证。')+'<div class="note-strip">'+icon('info')+'<span>这里是帮助理解语义的交互模型，不会编译或执行任意 C 代码。图中的地址仅为示意。</span></div><div class="tab-bar" role="navigation" aria-label="选择可视化实验">'+labs.map(l=>'<a href="#/lab/'+l[0]+'" class="tab-btn '+(type===l[0]?'active':'')+'" '+(type===l[0]?'aria-current="page"':'')+'>'+icon(l[1])+l[2]+'</a>').join('')+'</div><div id="lab-content"></div><div class="section-header"><h2>把观察变成自己的代码</h2></div><div class="surface tool-callout">'+({'pointer':'读完第 7—8 章，再运行 lessons/08_pointers.c；尝试先画图，再写指针交换函数。','memory':'读完第 10 章，完成 exercises/05_dynamic_array.c，再对照 src/task.c 的 reserve_one_more。','bits':'本项目完成位的掩码为 1u，紧急位为 2u。尝试只清除完成位，解释为什么另一个状态没有改变。','strings':'读完第 5 章，完成 exercises/03_string_copy.c。主项目标题容量为 96 字节；本实验缩小为 16 字节便于观察。'}[type])+' <a class="text-link" href="#/source">查看对应源码 '+icon('arrow')+'</a></div>';
    window.CTaskLabs.mount(document.getElementById('lab-content'), type);
  }
  function renderProject() {
    const done = taskData.items.filter(t=>(t.flags & 1)!==0).length;
    const shown = taskData.items.filter(t=>taskFilter==='all'||(taskFilter==='pending'?(t.flags&1)===0:(t.flags&1)!==0));
    main.innerHTML = heading('这就是你要亲手做出的项目。','先使用，再观察数据，最后打开 C 源码找出对应的函数。')+'<div class="note-strip">'+icon('info')+'<span>浏览器教学模拟：这些操作由网页实现，并映射到 C 项目接口。任务保存在当前浏览器；原生 C 程序需要本机编译运行。</span></div><div style="height:20px"></div><div class="project-grid"><section class="surface task-workspace"><div class="panel-title"><h2>我的学习任务</h2><span class="pill pill-green">'+done+' / '+taskData.items.length+' 已完成</span></div><form id="task-form" class="task-form"><input id="task-title" aria-label="新任务标题" placeholder="写下你的下一个小目标…" maxlength="200" required><select id="task-priority" aria-label="任务优先级"><option value="3">高优先级</option><option value="2" selected>普通</option><option value="1">低优先级</option></select><button type="submit" class="btn btn-primary">添加任务</button><p class="form-error" id="task-error" role="status" hidden></p></form><div class="task-toolbar"><div class="task-filter">'+[['all','全部'],['pending','待完成'],['done','已完成']].map(x=>'<button type="button" data-filter="'+x[0]+'" class="'+(taskFilter===x[0]?'active':'')+'">'+x[1]+'</button>').join('')+'</div><button class="btn btn-ghost btn-small" id="sort-tasks">按优先级排序 ↓</button></div><div id="task-list">'+(shown.length?shown.map(t=>'<div class="task-row '+((t.flags&1)?'is-done':'')+'"><input type="checkbox" data-done="'+t.id+'" aria-label="标记'+esc(t.title)+'完成" '+((t.flags&1)?'checked':'')+'><div class="task-content"><span class="task-title">'+esc(t.title)+'</span><span class="task-id">TASK #'+String(t.id).padStart(3,'0')+' · flags = '+t.flags+'</span></div><span class="priority priority-'+t.priority+'">'+({1:'低',2:'普通',3:'高'}[t.priority])+'</span><button class="icon-button '+((t.flags&2)?'urgent-on':'')+'" data-urgent="'+t.id+'" aria-label="切换'+esc(t.title)+'的紧急状态" aria-pressed="'+((t.flags&2)!==0)+'">'+icon('flag')+'</button><button class="icon-button" data-remove="'+t.id+'" aria-label="删除'+esc(t.title)+'">'+icon('trash')+'</button></div>').join(''):'<div class="empty-state">'+(taskData.items.length?'这个筛选条件下还没有任务。':'写下一个小目标，从这里开始。')+'</div>')+'</div><div class="task-bottom"><span>标题最多 95 个 UTF-8 字节<br>ID 表示身份，位置变化不改变 ID。</span><button class="text-link icon-button" id="export-tasks">'+icon('download')+'导出 CTASK 数据</button></div></section><div><section class="surface runtime-panel"><span class="eyebrow">UNDER THE HOOD</span><h2>你的操作，怎样改变 TaskList？</h2><p>把界面里的每一步，对应到一个 C 函数。</p><div class="runtime-stats">'+[['len',taskData.items.length],['cap',taskData.cap],['next_id',taskData.next_id]].map(x=>'<div class="runtime-stat"><small>'+x[0]+'</small><strong>'+x[1]+'</strong></div>').join('')+'</div>'+codePanel('typedef struct {\n    Task *items;\n    size_t len;\n    size_t cap;\n    unsigned long next_id;\n} TaskList;','include/task.h')+'<div class="operation-log" role="status">'+esc(lastOperation)+'</div></section><div class="surface source-link-card">想知道真实的扩容、查找和删除如何完成？<br><a href="#/source/'+data.sources.findIndex(s=>s.path==='src/task.c')+'">打开 task.c</a><span style="margin:0 9px;color:#c1c7d3">/</span><a href="#/doc/guide">阅读源码导读</a></div></div></div>';
    const apply = operation => { lastOperation=operation; persist('ctask-visual-tasks-v1',taskData); renderProject(); };
    document.getElementById('task-form').addEventListener('submit', ev=>{
      ev.preventDefault(); const title=document.getElementById('task-title').value; const priority=Number(document.getElementById('task-priority').value); const bytes=new TextEncoder().encode(title).length;
      const error=document.getElementById('task-error');
      if (!title.length||bytes>95||/[\x00-\x1f\x7f]/.test(title)) { error.hidden=false; error.textContent='请输入 1–95 个 UTF-8 字节，不包含控制字符的标题（当前 '+bytes+' 字节）。'; return; }
      if (taskData.items.length>=1000||taskData.next_id>=4294967295) { error.hidden=false; error.textContent='教学演示已达到数据上限。'; return; }
      const id=taskData.next_id++; if(taskData.items.length===taskData.cap) taskData.cap=taskData.cap?taskData.cap*2:8;
      taskData.items.push({id,title,priority,flags:0}); apply('task_add(&list, '+JSON.stringify(title)+', PRIORITY_'+({1:'LOW',2:'NORMAL',3:'HIGH'}[priority])+', &id); → #'+id);
    });
    main.querySelectorAll('[data-done]').forEach(el=>el.addEventListener('change',()=>{const t=taskData.items.find(x=>x.id===Number(el.dataset.done));t.flags=el.checked?t.flags|1:t.flags&~1;apply('task_set_done(&list, '+t.id+', '+el.checked+'); → flags = '+t.flags);}));
    main.querySelectorAll('[data-urgent]').forEach(el=>el.addEventListener('click',()=>{const t=taskData.items.find(x=>x.id===Number(el.dataset.urgent));t.flags^=2;apply('task_set_urgent(&list, '+t.id+', '+((t.flags&2)!==0)+'); → flags = '+t.flags);}));
    main.querySelectorAll('[data-remove]').forEach(el=>el.addEventListener('click',()=>{const id=Number(el.dataset.remove);taskData.items=taskData.items.filter(t=>t.id!==id);apply('task_remove(&list, '+id+'); // len 减 1，cap 与 next_id 保持不变');}));
    main.querySelectorAll('[data-filter]').forEach(el=>el.addEventListener('click',()=>{taskFilter=el.dataset.filter;renderProject();}));
    document.getElementById('sort-tasks').addEventListener('click',()=>{taskData.items.sort((a,b)=>b.priority-a.priority||a.id-b.id);apply('task_sort_by_priority(&list); // 优先级降序，同优先级 ID 升序');});
    document.getElementById('export-tasks').addEventListener('click',()=>{download('tasks.tsv','CTASK\t1\t'+taskData.next_id+'\n'+taskData.items.map(t=>[t.id,t.priority,t.flags,t.title].join('\t')+'\n').join(''));toast('已导出。原生程序用 --file tasks.tsv 启动后，选择菜单 8 读取。');});
  }
  const exerciseHints={
    '01_temperature':'让计算从一开始就使用浮点数。先预测 0、100、−40 摄氏度的结果。',
    '02_array_stats':'先检查指针和长度，用首元素初始化最值；成功后再提交结果，累加前检查范围。',
    '03_string_copy':'容量包括末尾的 NUL。先检查是否能完整放下，不足时保留原缓冲区。',
    '04_pointer_swap':'交换被指向的整数，而不是两个局部指针。考虑空指针和两个指针相同。',
    '05_dynamic_array':'分清 size 和 capacity；先用临时指针接收 realloc，成功后再更新容器。',
    '06_record_lookup':'从前向后遍历，返回匹配元素的借用地址。未找到返回 NULL，不返回局部对象的地址。'
  };
  function renderExercises(id) {
    const exercise=data.exercises.find(e=>e.id===id)||data.exercises[0];
    const command='.\\build.ps1 -Action exercise -Exercise '+exercise.id;
    main.innerHTML=heading('亲手写过，知识才属于你。','先读失败信息，补完 TODO，再运行自己的 C 程序。')+'<div class="exercise-layout"><aside class="surface exercise-list" aria-label="选择练习">'+data.exercises.map(e=>'<a href="#/exercises/'+e.id+'" class="'+(e.id===exercise.id?'active':'')+'"><small>EXERCISE '+e.id.slice(0,2)+'</small>'+esc(e.title)+'</a>').join('')+'</aside><section class="surface exercise-panel"><span class="eyebrow">EXERCISE '+exercise.id.slice(0,2)+'</span><h2>'+esc(exercise.title)+'</h2><p>'+exerciseHints[exercise.id]+'</p><div class="note-strip">'+icon('info')+'<span>这里可以编辑和下载代码草稿。网页不运行 C；请在源码项目目录使用下方命令编译自检。初始 TODO 版本返回 FAIL 是预期行为。</span></div><div class="editor-toolbar"><span class="pill">'+exercise.id+'.c</span><div><button class="btn btn-small" id="reset-exercise">恢复题目</button><button class="btn btn-small" id="toggle-solution">查看参考答案</button><button class="btn btn-primary btn-small" id="download-exercise">'+icon('download')+'下载代码</button></div></div><label class="sr-only" for="exercise-code">C 练习代码草稿</label><textarea class="code-editor" id="exercise-code" spellcheck="false" aria-label="C 练习代码草稿"></textarea><div class="command-box"><code>'+esc(command)+'</code><button id="copy-command">复制命令</button></div><p class="small muted" style="margin:14px 0 0">先把下载的文件放回 <code>exercises/</code> 对应路径，再运行命令。Linux/macOS：<code>sh build.sh exercise '+exercise.id+'</code>。草稿会自动保存在当前浏览器。</p><div id="reference-solution" hidden><div class="section-header"><h2>参考答案 · 先独立尝试再对照</h2></div>'+codePanel(exercise.solution,'solutions/'+exercise.id+'.c')+'</div></section></div>';
    const editor=document.getElementById('exercise-code');editor.value=typeof state.drafts[exercise.id]==='string'?state.drafts[exercise.id]:exercise.code;
    editor.addEventListener('input',()=>{state.drafts[exercise.id]=editor.value;save();});
    editor.addEventListener('keydown',ev=>{if(ev.key==='Tab'){ev.preventDefault();const start=editor.selectionStart,end=editor.selectionEnd;editor.setRangeText('    ',start,end,'end');editor.dispatchEvent(new Event('input'));}});
    document.getElementById('reset-exercise').addEventListener('click',()=>{if(editor.value!==exercise.code&&!window.confirm('恢复初始题目会替换当前草稿，继续吗？'))return;editor.value=exercise.code;state.drafts[exercise.id]=exercise.code;save();toast('已恢复初始题目');});
    document.getElementById('toggle-solution').addEventListener('click',ev=>{const box=document.getElementById('reference-solution');box.hidden=!box.hidden;ev.currentTarget.textContent=box.hidden?'查看参考答案':'收起参考答案';});
    document.getElementById('download-exercise').addEventListener('click',()=>download(exercise.id+'.c',editor.value));
    document.getElementById('copy-command').addEventListener('click',()=>copy(command));
  }
  function renderSource(part, lessonId) {
    const lesson=part==='lesson'?(data.lessons.find(l=>l.id===lessonId)||data.lessons[0]):null;
    const selected=lesson?null:(data.sources[Number(part)]||data.sources[0]);
    const code=lesson?lesson.code:selected.code, label=lesson?'lessons/'+lesson.id+'.c':selected.path;
    main.innerHTML=heading('真实的 C 源码，就在这里。','每个模块都有职责，每个实验都可以独立运行。','<a class="btn" href="downloads/c-task-lab.zip" download>'+icon('download')+'下载 C 源码包</a>')+'<div class="source-layout"><aside class="surface source-tree"><h3>PROJECT FILES</h3>'+data.sources.map((s,i)=>'<a href="#/source/'+i+'" class="source-file-link '+(selected===s?'active':'')+'">'+esc(s.path)+'</a>').join('')+'<h3 style="margin-top:20px">16 个独立实验</h3>'+data.lessons.map(l=>'<a href="#/source/lesson/'+l.id+'" class="source-file-link '+(lesson===l?'active':'')+'">'+esc(l.id)+'.c</a>').join('')+'</aside><section class="source-view">'+codePanel(code,label)+'<button id="download-source" class="btn btn-small">'+icon('download')+'下载当前文件</button><div class="surface tool-callout"><strong>从看懂一行，到追踪整个功能</strong>进入项目目录，运行 <code>.\\build.ps1 -Action test</code> 验证核心；运行 <code>.\\build.ps1 -Action run</code> 使用原生 C 应用。'+(lesson?'<br>当前实验命令：<code>.\\build.ps1 -Action lesson -Lesson '+lesson.id+'</code>':'')+'<br><a class="text-link" href="#/doc/guide">打开源码阅读指南 '+icon('arrow')+'</a></div></section></div>';
    document.getElementById('download-source').addEventListener('click',()=>download(label.split('/').pop(),code));
  }
  function renderDoc(name) {
    const titles={start:'第一课：让程序听你的话',guide:'完整项目源码导读',roadmap:'C 语言知识地图',glossary:'术语与常见误区',readme:'项目说明与编译方法',stages:'同一个项目，逐步长大',exercises:'六道练习的完整题面',lessons:'16 个独立实验的观察指南'};
    if(!data.docs[name])name='start';
    main.innerHTML=heading(titles[name],'遇到一个具体问题，再查一个具体知识点。')+'<article class="surface doc-content markdown">'+markdown(data.docs[name])+'</article>';
  }
  const labels={overview:'学习总览',course:'章节课程',lab:'可视化实验',project:'项目实战',exercises:'动手练习',source:'源码与工具',doc:'学习资料'};
  function render() {
    let parts;
    try { parts=decodeURIComponent(location.hash.replace(/^#\/?/, '')).split('/'); } catch(_){ parts=['overview']; }
    const page=Object.hasOwn(labels,parts[0])?parts[0]:'overview';
    document.title=labels[page]+' · C 语言任务工坊';
    document.getElementById('page-label').textContent=labels[page];
    document.querySelectorAll('[data-nav]').forEach(link=>{const active=link.dataset.nav===page;link.classList.toggle('active',active);if(active)link.setAttribute('aria-current','page');else link.removeAttribute('aria-current');});
    document.getElementById('sidebar').classList.remove('open');document.getElementById('menu-toggle').setAttribute('aria-expanded','false');
    if(page==='course')renderCourse(parts[1]);else if(page==='lab')renderLab(parts[1]);else if(page==='project')renderProject();else if(page==='exercises')renderExercises(parts[1]);else if(page==='source')renderSource(parts[1],parts[2]);else if(page==='doc')renderDoc(parts[1]);else overview();
    updateProgress();window.scrollTo(0,0);
  }
  document.addEventListener('click',ev=>{
    if(ev.target.closest('.skip-link')){ev.preventDefault();main.focus({preventScroll:true});main.scrollIntoView({block:'start'});return;}
    const button=ev.target.closest('[data-copy-code]');if(button){const code=button.closest('.code-panel').querySelector('.code-content');if(code)copy(code.textContent);}
    if(!ev.target.closest('.search-wrap'))document.getElementById('search-results').hidden=true;
    if(document.getElementById('sidebar').classList.contains('open')&&!ev.target.closest('.sidebar')&&!ev.target.closest('#menu-toggle')){document.getElementById('sidebar').classList.remove('open');document.getElementById('menu-toggle').setAttribute('aria-expanded','false');}
  });
  document.getElementById('menu-toggle').addEventListener('click',ev=>{const open=document.getElementById('sidebar').classList.toggle('open');ev.currentTarget.setAttribute('aria-expanded',String(open));});
  const search=document.getElementById('global-search'),results=document.getElementById('search-results');
  search.addEventListener('input',()=>{
    const query=search.value.trim().toLowerCase();if(!query){results.hidden=true;return;}
    const matches=data.chapters.filter(c=>(c.title+' '+c.markdown).toLowerCase().includes(query)).slice(0,7);results.hidden=false;
    results.innerHTML=matches.length?matches.map(c=>'<a href="#/course/'+c.id+'"><span>第 '+c.id+' 章 · '+esc(c.title)+'</span><small>'+esc(c.markdown.replace(/[#*`]/g,'').slice(0,75))+'…</small></a>').join(''):'<p class="search-empty">没有匹配章节，试试“指针”“文件”或“内存”。</p>';
  });
  results.addEventListener('click',()=>{results.hidden=true;search.value='';});
  document.addEventListener('keydown',ev=>{if((ev.ctrlKey||ev.metaKey)&&ev.key.toLowerCase()==='k'){ev.preventDefault();search.focus();}if(ev.key==='Escape'){results.hidden=true;document.getElementById('sidebar').classList.remove('open');document.getElementById('menu-toggle').setAttribute('aria-expanded','false');}});
  window.addEventListener('hashchange',render);
  paintIcons();render();
})();
