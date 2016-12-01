#!/usr/bin/env python

import cgi
import re, sys
import pprint

class Function:
  def __init__(self, name):
    self.name = name;
    self.calls = []
    self.cost = 0

  def Calculate(self):
    total = 1;
    for c in self.calls:
      c.Calculate()
      total += c.cost
    self.cost = total

  def Print(self, indent):
    if len(self.calls) > 0:
      print "  " * indent + self.name + " [%d] {" % self.cost
      for c in self.calls:
        c.Print(indent+1)
      print "  " * indent + '}'
    else:
      assert self.cost == 1
      print "  " * indent + self.name

  def PrintHTML(self, out, indent):
    if len(self.calls) > 0:
      out.write("  " * indent + '<li>' + cgi.escape(self.name) + ' [%d]<ul>\n' % self.cost)
      for c in self.calls:
        c.PrintHTML(out, indent+1)
      out.write("  " * indent + '</ul></li>\n')
    else:
      out.write("  " * indent + '<li>' + cgi.escape(self.name) + "</li>\n")


def load_symbol():
    SYMBOL = re.compile('([0-9a-f]+) [tT] (.*)')
    symbols = {}
    with open('symbol.txt') as syms:
        for line in syms:
            m = SYMBOL.match(line)
            if m:
                symbols[int(m.group(1), 16)] = m.group(2)
    return symbols

symbols = load_symbol()

def get_func_name(addr):
    return symbols.get(int(addr, 16), 'unknown')

CALL = re.compile('^(ENTER|EXIT): +0x([0-9a-f]+)')
indent = 0
stack = [Function('main')]

with open('trace.txt') as trace:
  for line in trace:
    m = CALL.search(line)
    if m:
      if m.group(1) == 'ENTER':
        func_name = get_func_name(m.group(2))
        # print "  " * indent + func_name + ' {'
        if len(stack) <= indent+1:
          stack.append(None)
        indent += 1
        stack[indent] = Function(func_name)
        stack[indent-1].calls.append(stack[indent])
      else:
        indent -= 1
        if indent < 0:
          print line
        # print "  " * indent + '}'

stack[0].Calculate()
stack[0].Print(0)
if indent != 0:
  print "INBALANCE!"

with open('calltree.html', 'w') as out:
  out.write('''<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Call Tree</title>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/jstree/3.2.1/themes/default/style.min.css" />
</head>
<body>
  <input type="text" value="" id="demo_q" style="width: 450px;" placeholder="Search" />
  <div id="jstree">
    <ul>
''')

  stack[0].PrintHTML(out, 0)

  out.write('''
    </ul>
  </div>

  <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/1.12.1/jquery.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/jstree/3.2.1/jstree.min.js"></script>
  <script>
  $(function () {
    var to = false;

    $('#demo_q').keyup(function () {
      if(to) { clearTimeout(to); }
      to = setTimeout(function () {
        var v = $('#demo_q').val();
        $('#jstree').jstree(true).search(v);
      }, 250);
    });

    $('#jstree').jstree({
      "plugins" : [ "search"]
    });
  });
  </script>
</body>
</html>''');
