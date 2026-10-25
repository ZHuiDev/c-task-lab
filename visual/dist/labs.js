(function () {
  'use strict';

  var escapeHtml = function (value) {
    return String(value).replace(/[&<>"']/g, function (character) {
      return { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[character];
    });
  };

  function shell(container, title, description, content) {
    container.innerHTML = '<section class="clab" aria-label="' + escapeHtml(title) + '">' +
      '<div class="clab-heading"><div><span class="clab-eyebrow">交互实验</span><h2>' + title + '</h2></div></div>' +
      '<p class="clab-intro">' + description + '</p>' + content + '</section>';
  }

  function pointer(container) {
    var step = 0;
    var lines = ['int score = 20;', 'int *p = &score;', 'int copied = *p;', '*p = 35;', 'copied += 1;'];
    var explanations = [
      '点击“下一步”，从第一行开始观察变量。地址是教学示意值，本页没有编译或运行 C 代码。',
      '创建 int 变量 score，值为 20。&score 表示这个变量的地址。',
      '指针 p 保存 score 的地址。p 的值是地址，*p 访问这个地址上的 int 对象。',
      '把 *p 当前的值 20 复制到 copied。copied 是独立的 int 变量。',
      '通过 *p 写入 35，修改的是 score 本身。先前复制出来的 copied 仍然是 20。',
      'copied 自己加 1，成为 21。score 仍为 35：复制值不会建立持续关联。'
    ];
    shell(container, '指针：地址、解引用与复制', '逐行推进，观察同一块内存如何被变量名和指针访问。',
      '<div class="clab-layout"><div class="clab-code-panel"><div class="clab-panel-label">示例 C 代码</div><pre class="clab-code" data-code></pre></div>' +
      '<div class="clab-figure"><div class="clab-pointer-scene" data-scene></div><div class="clab-inline-note">地址为示意地址，不代表本机实际内存布局。</div></div></div>' +
      '<div class="clab-toolbar"><button type="button" class="clab-button clab-primary" data-next>下一步</button><button type="button" class="clab-button" data-reset>重置</button><span class="clab-step" data-step></span></div>' +
      '<p class="clab-status" role="status" aria-live="polite" data-status></p>');

    var code = container.querySelector('[data-code]');
    var scene = container.querySelector('[data-scene]');
    var next = container.querySelector('[data-next]');
    function box(name, value, address, className, exists) {
      return '<div class="clab-memory-box ' + className + (exists ? '' : ' clab-pending') + '"><span class="clab-var">' + name + '</span><strong>' + value + '</strong><span class="clab-address">' + address + '</span></div>';
    }
    function render() {
      code.innerHTML = lines.map(function (line, index) {
        return '<span class="clab-code-line' + (step === index + 1 ? ' clab-current' : '') + (step > index + 1 ? ' clab-executed' : '') + '"><span class="clab-line-number" aria-hidden="true">' + (index + 1) + '</span>' + escapeHtml(line) + '</span>';
      }).join('');
      var score = step >= 4 ? '35' : '20';
      var copied = step >= 5 ? '21' : '20';
      scene.innerHTML = box('p', step >= 2 ? '0x1000' : '未创建', 'p 自身位于 0x2000', 'clab-pointer-box', step >= 2) +
        '<div class="clab-arrow' + (step >= 2 ? ' clab-arrow-visible' : '') + '" aria-hidden="true"><span>*p</span><svg viewBox="0 0 130 55" fill="none"><path d="M4 27H120M108 17L120 27L108 37" stroke="currentColor" stroke-width="3" stroke-linecap="round" stroke-linejoin="round"/></svg></div>' +
        box('score', step >= 1 ? score : '未创建', '&score = 0x1000', 'clab-score-box' + (step === 4 ? ' clab-changed' : ''), step >= 1) +
        box('copied', step >= 3 ? copied : '未创建', '&copied = 0x1004', 'clab-copy-box' + (step === 5 ? ' clab-changed' : ''), step >= 3) +
        '<div class="clab-pointer-caption">' + (step >= 2 ? 'p → score：两个访问方式，同一个对象' : '内存对象将在执行相应语句后出现') + '</div>';
      container.querySelector('[data-step]').textContent = '步骤 ' + step + ' / ' + lines.length;
      container.querySelector('[data-status]').textContent = explanations[step];
      next.disabled = step === lines.length;
      next.textContent = step === lines.length ? '演示完成' : '下一步';
    }
    next.addEventListener('click', function () { if (step < lines.length) { step += 1; render(); } });
    container.querySelector('[data-reset]').addEventListener('click', function () { step = 0; render(); next.focus(); });
    render();
  }

  function memory(container) {
    var items = [], capacity = 0, generation = 0, nextValue = 10, borrowed = null, migration = null;
    var status = '空数组尚未分配空间：len = 0，cap = 0。添加第一项时分配 4 个 int 的空间。';
    function baseAddress(gen) { return '0x' + (0x2000 + gen * 0x1000).toString(16).toUpperCase(); }
    shell(container, '动态数组：长度、容量与扩容', '这是一份可操作的内存模型；分配地址和迁移过程用于说明概念，并非真实 C 内存执行结果。',
      '<div class="clab-metrics"><div><span>当前元素 len</span><strong data-length>0</strong></div><div><span>可容纳元素 cap</span><strong data-capacity>0</strong></div><div><span>占用比例</span><strong data-ratio>0 / 0</strong></div></div>' +
      '<div class="clab-array-panel"><div class="clab-array-title"><span>当前数组</span><code data-base>未分配</code></div><div class="clab-array" data-array></div><div class="clab-array-legend"><span><i class="clab-legend-used"></i>已使用</span><span><i class="clab-legend-empty"></i>预留容量</span></div></div>' +
      '<div class="clab-migration" data-migration hidden></div>' +
      '<div class="clab-toolbar"><button type="button" class="clab-button clab-primary" data-add>添加一项</button><button type="button" class="clab-button" data-remove>移除末项</button><button type="button" class="clab-button" data-borrow>借用首项地址</button><button type="button" class="clab-button" data-reset>重置</button></div>' +
      '<div class="clab-borrow" data-borrow-state></div><p class="clab-status" role="status" aria-live="polite" data-status></p>' +
      '<details class="clab-details"><summary>对应到 C：为什么不能一直保存旧指针？</summary><p>扩容可能改变数组所在的地址。若重新分配成功并替换了原数组，原来指向数组或其中元素的借用指针就不应继续使用；应从新数组重新获取。使用 realloc 时，即使新地址的数值看起来相同，也应以返回的新指针为准。此模型特意让扩容迁移到不同的示意地址。</p><p>len 表示有效元素个数，cap 表示已分配的元素容量。这里移除末项只减少 len，不缩小 cap。重置演示则释放整个模型数组。</p></details>');
    var addButton = container.querySelector('[data-add]');
    var removeButton = container.querySelector('[data-remove]');
    var borrowButton = container.querySelector('[data-borrow]');
    function cell(value, index, filled, old) {
      return '<div class="clab-cell' + (filled ? ' clab-cell-filled' : '') + (old ? ' clab-cell-old' : '') + '"><span>[' + index + ']</span><strong>' + (filled ? value : '—') + '</strong></div>';
    }
    function render() {
      container.querySelector('[data-length]').textContent = items.length;
      container.querySelector('[data-capacity]').textContent = capacity;
      container.querySelector('[data-ratio]').textContent = items.length + ' / ' + capacity;
      container.querySelector('[data-base]').textContent = capacity ? baseAddress(generation) + '（示意）' : '未分配';
      var cells = '';
      for (var i = 0; i < capacity; i += 1) cells += cell(items[i], i, i < items.length, false);
      container.querySelector('[data-array]').innerHTML = capacity ? cells : '<p class="clab-empty">没有已分配的格子。点击“添加一项”。</p>';
      var migrationBox = container.querySelector('[data-migration]');
      migrationBox.hidden = !migration;
      if (migration) {
        migrationBox.innerHTML = '<div class="clab-migration-title">扩容：cap ' + migration.capacity + ' → ' + capacity + ' · 复制 ' + migration.items.length + ' 个有效元素</div><div class="clab-array-title"><span>旧数组：已释放，不能再访问</span><code>' + baseAddress(migration.generation) + '</code></div><div class="clab-array clab-old-array">' + migration.items.map(function (value, index) { return cell(value, index, true, true); }).join('') + '</div><p>旧内容迁移到上方的新数组，再添加新元素。指向旧数组的借用指针失效。</p>';
      }
      var borrowBox = container.querySelector('[data-borrow-state]');
      if (!borrowed) {
        borrowBox.className = 'clab-borrow';
        borrowBox.innerHTML = '<strong>借用指针 first</strong><span>尚未借用。先添加元素，再点击“借用首项地址”。</span>';
      } else {
        var stale = borrowed.generation !== generation;
        var exists = items.length > 0;
        borrowBox.className = 'clab-borrow' + (stale || !exists ? ' clab-borrow-invalid' : ' clab-borrow-valid');
        borrowBox.innerHTML = '<strong>first = ' + baseAddress(borrowed.generation) + '</strong><span>' + (stale ? '已失效：数组扩容后迁移。不能解引用，请重新借用。' : exists ? '有效借用：*first = ' + items[0] + '，指向当前数组的第 0 项。' : '已无有效首项：len = 0。此地址仍在已分配空间内，但不应作为当前元素访问。') + '</span>';
      }
      addButton.disabled = items.length >= 16;
      addButton.textContent = items.length >= 16 ? '已达演示上限 16 项' : '添加一项';
      removeButton.disabled = !items.length;
      borrowButton.disabled = !items.length;
      container.querySelector('[data-status]').textContent = status;
    }
    addButton.addEventListener('click', function () {
      if (items.length >= 16) return;
      var firstAllocation = capacity === 0;
      migration = null;
      if (items.length === capacity) {
        if (capacity) migration = { items: items.slice(), capacity: capacity, generation: generation };
        capacity = capacity ? capacity * 2 : 4;
        generation += 1;
      }
      var value = nextValue;
      nextValue += 10;
      items.push(value);
      status = migration ? '空间已满，容量翻倍到 ' + capacity + '，迁移 ' + migration.items.length + ' 个元素后写入 ' + value + '。旧数组地址不可再使用。' : firstAllocation ? '已分配 4 个 int 的空间，并写入首项 ' + value + '。可借用首项地址，再继续添加到第 5 项，观察扩容。' : '在预留空间写入 ' + value + '。len 增加到 ' + items.length + '，cap 保持 ' + capacity + '。';
      render();
    });
    removeButton.addEventListener('click', function () {
      if (!items.length) return;
      var removed = items.pop();
      migration = null;
      status = '移除了末项 ' + removed + '。len 减少到 ' + items.length + '，cap 保持 ' + capacity + '，没有发生缩容。';
      render();
    });
    borrowButton.addEventListener('click', function () {
      if (!items.length) return;
      borrowed = { generation: generation };
      status = '令 first = &items[0]，保存当前首项的示意地址。继续添加，直到下一次扩容，观察这个借用指针。';
      render();
    });
    container.querySelector('[data-reset]').addEventListener('click', function () {
      items = []; capacity = 0; generation = 0; nextValue = 10; borrowed = null; migration = null;
      status = '已重置：数组空间和借用指针均已清除，len = 0，cap = 0。';
      render(); addButton.focus();
    });
    render();
  }

  function bits(container) {
    var flags = 0;
    var operation = 'unsigned int flags = 0u;';
    var explanation = '两个标记均未设置。勾选开关，观察位掩码如何改变 flags。';
    shell(container, '位运算：一个整数，多个开关', '用不同的二进制位保存“完成”和“紧急”状态。下方始终展示 flags 的低 8 位。',
      '<div class="clab-switches"><label class="clab-switch"><input type="checkbox" data-complete><span><strong>完成</strong><small>掩码 1u · 第 0 位</small></span></label><label class="clab-switch"><input type="checkbox" data-urgent><span><strong>紧急</strong><small>掩码 2u · 第 1 位</small></span></label></div>' +
      '<div class="clab-bit-panel"><div class="clab-array-title"><span>flags 的低 8 位</span><strong data-decimal>十进制 0</strong></div><div class="clab-bits" data-bits></div><div class="clab-bit-labels"><span>高位</span><span>第 1 位：紧急 · 第 0 位：完成</span></div></div>' +
      '<div class="clab-code-panel"><div class="clab-panel-label">刚才的 C 操作</div><pre class="clab-code clab-code-padded"><code data-operation></code></pre></div><p class="clab-status" role="status" aria-live="polite" data-status></p>' +
      '<div class="clab-tests" data-tests></div><div class="clab-toolbar"><button type="button" class="clab-button" data-reset>全部清除</button></div>' +
      '<details class="clab-details"><summary>为什么设置用 |，清除用 &amp; 和 ~？</summary><p>flags |= mask 将掩码为 1 的位设为 1，保留其余位。flags &amp;= ~mask 将掩码为 1 的位清为 0，也保留其余位。测试是否设置使用 (flags &amp; mask) != 0u。</p><p>这里的 flags 是 unsigned int，实际通常多于 8 位；图中只画低 8 位。~1u 的低 8 位是 11111110，~2u 的低 8 位是 11111101。</p></details>');
    var complete = container.querySelector('[data-complete]');
    var urgent = container.querySelector('[data-urgent]');
    function render() {
      complete.checked = Boolean(flags & 1);
      urgent.checked = Boolean(flags & 2);
      var cells = '';
      for (var bit = 7; bit >= 0; bit -= 1) {
        var enabled = Boolean(flags & (1 << bit));
        cells += '<div class="clab-bit' + (enabled ? ' clab-bit-on' : '') + (bit < 2 ? ' clab-bit-assigned' : '') + '"><span>bit ' + bit + '</span><strong>' + (enabled ? '1' : '0') + '</strong><small>' + (bit === 0 ? '完成' : bit === 1 ? '紧急' : '未用') + '</small></div>';
      }
      container.querySelector('[data-bits]').innerHTML = cells;
      container.querySelector('[data-decimal]').textContent = '十进制 ' + flags;
      container.querySelector('[data-operation]').textContent = operation;
      container.querySelector('[data-status]').textContent = explanation + ' 当前 flags = ' + flags + '，二进制 ' + flags.toString(2).padStart(8, '0') + '。';
      container.querySelector('[data-tests]').innerHTML = '<div><code>(flags &amp; 1u) != 0u</code><strong>' + (flags & 1 ? '真：已完成' : '假：未完成') + '</strong></div><div><code>(flags &amp; 2u) != 0u</code><strong>' + (flags & 2 ? '真：紧急' : '假：普通') + '</strong></div>';
    }
    function toggle(mask, enabled, name) {
      flags = enabled ? flags | mask : flags & ~mask;
      operation = enabled ? 'flags |= ' + mask + 'u;' : 'flags &= ~' + mask + 'u;';
      explanation = (enabled ? '使用按位或，设置' : '使用按位与和取反，清除') + '“' + name + '”位；另一个标记保持原值。';
      render();
    }
    complete.addEventListener('change', function () { toggle(1, complete.checked, '完成'); });
    urgent.addEventListener('change', function () { toggle(2, urgent.checked, '紧急'); });
    container.querySelector('[data-reset]').addEventListener('click', function () { flags = 0; operation = 'flags = 0u;'; explanation = 'flags 赋值为 0，全部标记均已清除。'; render(); });
    render();
  }

  function strings(container) {
    var encoder = new TextEncoder();
    var stored = '任务 A';
    var draft = stored;
    var status = '数组已保存“任务 A”。可以输入新内容，再点击“写入数组”。';
    var rejected = false;
    shell(container, '字符串：看见 UTF-8 的每个字节', 'char title[16] 有 16 个字节；C 字符串需要一个结尾的 NUL，因此内容最多占 15 字节。',
      '<form class="clab-string-form" data-form><label class="clab-field-label">待写入的标题<input class="clab-text-input" type="text" data-input autocomplete="off" spellcheck="false" aria-label="待写入的标题，最多15个UTF-8内容字节"></label><p class="clab-field-help">支持 ASCII、中文及其他 UTF-8 文本。先输入，再写入；超限会拒绝整次写入，保留原内容。</p><div class="clab-draft-count" data-draft-count></div><div class="clab-toolbar"><button type="submit" class="clab-button clab-primary">写入数组</button><button type="button" class="clab-button" data-example="ascii">ASCII 示例</button><button type="button" class="clab-button" data-example="chinese">中文示例</button><button type="button" class="clab-button" data-example="over">试试超限</button></div></form>' +
      '<div class="clab-metrics clab-string-metrics"><div><span>数组容量 sizeof title</span><strong>16 <small>字节</small></strong></div><div><span>已保存 strlen(title)</span><strong data-strlen></strong></div><div><span>已保存文本的码点数</span><strong data-codepoints></strong></div></div>' +
      '<div class="clab-array-panel"><div class="clab-array-title"><span>char title[16] · 每格 1 字节</span><code data-saved></code></div><div class="clab-byte-array" data-bytes></div><div class="clab-array-legend"><span><i class="clab-legend-used"></i>UTF-8 内容字节</span><span><i class="clab-legend-null"></i>结尾 NUL</span><span><i class="clab-legend-empty"></i>剩余空间</span></div></div>' +
      '<p class="clab-status" role="status" aria-live="polite" data-status></p><div class="clab-inline-note">strlen 计算第一个 NUL 之前的字节数，不包含 NUL。本模型每次成功写入前先清零，因此剩余空间也显示 00。码点数使用 Array.from 计算；组合符号和某些表情可能含多个码点，因此它也不一定等于视觉上的字符数。</div>');
    var input = container.querySelector('[data-input]');
    input.value = draft;
    function measureDraft() {
      var length = encoder.encode(draft).length;
      var invalid = length > 15 || draft.indexOf('\0') !== -1;
      var counter = container.querySelector('[data-draft-count]');
      counter.className = 'clab-draft-count' + (invalid ? ' clab-count-invalid' : '');
      counter.textContent = '待写入：' + length + ' 字节 · ' + Array.from(draft).length + ' 个码点' + (draft.indexOf('\0') !== -1 ? ' · 含 NUL，不能作为本实验的完整标题' : length > 15 ? ' · 超出 15 字节上限' : ' · 加结尾 NUL 共 ' + (length + 1) + ' / 16 字节');
      input.setAttribute('aria-invalid', invalid ? 'true' : 'false');
    }
    function renderStored() {
      var bytes = encoder.encode(stored);
      container.querySelector('[data-strlen]').innerHTML = bytes.length + ' <small>字节</small>';
      container.querySelector('[data-codepoints]').textContent = Array.from(stored).length;
      container.querySelector('[data-saved]').textContent = '已保存：' + JSON.stringify(stored);
      var cells = '';
      for (var i = 0; i < 16; i += 1) {
        var isData = i < bytes.length, isNul = i === bytes.length;
        var hex = (isData ? bytes[i] : 0).toString(16).toUpperCase().padStart(2, '0');
        cells += '<div class="clab-byte' + (isData ? ' clab-byte-data' : isNul ? ' clab-byte-nul' : '') + '" aria-label="第 ' + i + ' 字节：0x' + hex + (isNul ? '，字符串结束 NUL' : isData ? '，内容' : '，剩余空间，初始化为零') + '"><span>[' + i + ']</span><strong>' + hex + '</strong><small>' + (isNul ? 'NUL' : isData ? 'UTF-8' : '空闲') + '</small></div>';
      }
      container.querySelector('[data-bytes]').innerHTML = cells;
      var statusBox = container.querySelector('[data-status]');
      statusBox.className = 'clab-status' + (rejected ? ' clab-status-error' : '');
      statusBox.textContent = status;
    }
    function save() {
      var bytes = encoder.encode(draft);
      rejected = bytes.length > 15 || draft.indexOf('\0') !== -1;
      if (rejected) status = draft.indexOf('\0') !== -1 ? '已拒绝写入：标题含有 NUL，会提前终止 C 字符串。已保存内容保持不变。' : '已拒绝写入：内容占 ' + bytes.length + ' 字节，加 NUL 后需要 ' + (bytes.length + 1) + ' 字节，超过 16 字节容量。没有截断，已保存内容保持不变。';
      else { stored = draft; status = '写入成功：保存 ' + bytes.length + ' 个内容字节，随后写入 1 个结尾 NUL。strlen(title) = ' + bytes.length + '。'; }
      measureDraft(); renderStored();
    }
    input.addEventListener('input', function () { draft = input.value; measureDraft(); });
    container.querySelector('[data-form]').addEventListener('submit', function (event) { event.preventDefault(); draft = input.value; save(); });
    container.querySelectorAll('[data-example]').forEach(function (button) {
      button.addEventListener('click', function () {
        draft = button.dataset.example === 'ascii' ? 'Learn C' : button.dataset.example === 'chinese' ? '学习C语言' : '学习C语言很有趣';
        input.value = draft; save();
      });
    });
    measureDraft(); renderStored();
  }

  window.CTaskLabs = {
    mount: function (container, type) {
      if (!container || typeof container.querySelector !== 'function') throw new TypeError('CTaskLabs.mount 需要一个 DOM 容器。');
      var labs = { pointer: pointer, memory: memory, bits: bits, strings: strings };
      if (!Object.prototype.hasOwnProperty.call(labs, type)) {
        container.innerHTML = '<p class="clab-status">没有找到这个实验。可用实验：pointer、memory、bits、strings。</p>';
        return;
      }
      labs[type](container);
    }
  };
}());
