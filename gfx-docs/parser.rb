require_relative 'model/directive'

class Parser
  GFX_DOCSTR_EXP = /\(%([^%]+)%\)/m
  DIRECTIVE_EXP = /\\([^\s]+)\s+\"((\\.|[^\"])*)\"/m
  
  attr_reader :string
  
  def initialize(string)
    @string = string
  end
  
  def go
    doc_strs = @string.scan(GFX_DOCSTR_EXP).map {|matches| matches[0].strip}
    doc_strs.map do |doc_str|
      doc_str.scan(DIRECTIVE_EXP).map do |match|
        name = match[0]
        contents = match[1].gsub('\"', '"')
        Directive.new(name, contents)
      end
    end
  end
end