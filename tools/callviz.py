#!/usr/bin/python

# functions not shown in graph
STOP = ['min', 'max', 'imin',
        'splx', 'splimp', 'splnet',
        'malloc', 'free',
        'htons', 'htonl', 'ntohs', 'ntohl',
        'bzero', 'bcmp', 'bcopy', 'ovbcopy',
        'panic', 'printf',
        'tsleep',
        ]

NOFOLLOW = [ 'sowakeup', 'sohasoutofband', 'rt_msg1' ]

GROUP_BY_FILE = 1
SHOW_FILE = 2

class Function(object):
  def __init__(self, name, source):
    self.name = name
    self.source = source
    self.calls = set()

  def add(self, call):
    self.calls.add(call)


class Dot(object):
  def __init__(self, graph, files, nofollow, stop):
    self.graph = graph
    self.files = files
    self.sources = set()
    self.known = {}
    self.undef = {}
    self.nofollow = NOFOLLOW[:]
    self.stop = STOP[:]
    if nofollow:
      self.nofollow.extend(nofollow)
    if stop:
      self.stop.extend(stop)

  def run(self, func):
    if func in self.graph:
      fn = self.graph[func]
      if func in self.known:
        return
      self.known[func] = set()
      self.sources.add(fn.source)
      if func in self.nofollow:
        return
      for call in fn.calls:
        if not call in self.stop:
          self.known[func].add(call)
          self.run(call)
    else:
      if func in self.undef:
        self.undef[func] += 1
      else:
        self.undef[func] = 1

  def printGraph(self, style):
    print 'digraph G {'
    print '  rankdir = LR;'
    for func in self.known:
      for call in self.known[func]:
        print '  "%s" -> "%s";' % (func, call)
    if style == GROUP_BY_FILE:
      self.printSubGraph()
    elif style == SHOW_FILE:
      for func in self.known:
        fn = self.graph[func]
        if func in self.nofollow:
          print '  "%s"[label="%s\\n%s",style=filled];' % (func, func, fn.source)
        else:
          print '  "%s"[label="%s\\n%s"];' % (func, func, fn.source)
    else:
      for f in self.nofollow:
        if f in self.known:
          print '  "%s" [style=filled];' % f
    for f in self.undef:
      print '  "%s" [style=filled,fillcolor=green];' % f
    print '}'

  def printSubGraph(self):
    count = 0
    for source in self.sources:
      count += 1
      print 'subgraph cluster%d {' % count
      print '  label = \"%s\";' % source
      print '  fontcolor=\"blue\";'
      for func in self.files[source]:
        if func in self.known:
          if func in self.nofollow:
            print '  "%s" [style=filled];' % func
          else:
            print '  "%s";' % func
      print '}'


class Graph(object):
  def __init__(self, callgraph):
    self.graph = {}
    self.files = {}
    with open(callgraph) as f:
      for line in f:
        line = line.rstrip()
        if line[0] == '/':
          source = line[1:]
          self.files[source] = set()
        elif line.startswith('\tfunc '):
          func = line[6:]
          self.graph[func] = Function(func, source)
          self.files[source].add(func)
        elif line.startswith('\t\tcall '):
          call = line[7:]
          self.graph[func].add(call)

  def show(self, func, style=None, nofollow=None, stop=None):
    dot = Dot(self.graph, self.files, nofollow, stop)
    dot.run(func)
    dot.printGraph(style)


if __name__ == '__main__':
  g = Graph('callgraph.txt')
  g.show('ip_output')
  g.show('tcp_output', GROUP_BY_FILE, nofollow=['ip_output'])
  g.show('ipintr', SHOW_FILE, nofollow=['ip_output'], stop=['m_free', 'm_freem'])
  g.show('tcp_input',
         nofollow=['ip_output', 'tcp_output'],
         stop=['m_free', 'm_freem', 'wakeup'])

