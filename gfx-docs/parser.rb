class Directive
  attr_accessor :name
  attr_accessor :contents
  
  def initialize(name, contents)
    @name = name
    @contents = contents
  end
end

class Parser
  GFX_DOCSTR_EXP = /\(%([^%]+)%\)/
  DIRECTIVE_EXP = /\\([^\s]+)\s\"((\\.|[^\"])*)\"/
  
  attr_reader :string
  
  def initialize(string)
    @string = string
  end
  
  def parse
    doc_strs = @string.scan(GFX_DOCSTR_EXP).map {|matches| matches[0].strip}
    doc_strs.map {|doc_str|
      doc_str.scan(DIRECTIVE_EXP).map {|d| Directive.new(d[0], d[1].gsub('\"', '"')) }
    }
  end
end