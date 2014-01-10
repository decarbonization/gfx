class Directive
  attr_accessor :name
  attr_accessor :contents
  
  def initialize(name, contents)
    @name = name
    @contents = contents
  end
end