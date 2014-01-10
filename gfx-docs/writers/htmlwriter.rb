require 'json'
require 'erb'
require 'ostruct'
require_relative 'outputwriter'

class HTMLWriter < OutputWriter
  def self.generate_module(the_module, destination)
    renderer = ERB.new(File.read("./templates/module.rhtml"))
    ns = OpenStruct.new(the_module: the_module, items: the_module.doc_infos)
    result = renderer.run(ns.instance_eval { binding })
    File.open(destination, "w") do |f|
      f.write(result)
    end
  end
  
  def self.generate(modules, destination)
    throw NotImplementedError unless destination.is_a? String
    
    Dir.mkdir(destination) unless File.exist? destination
    
    modules.each do |the_module|
      generate_module(the_module, File.join(destination, the_module.name + ".html"))
    end
    ""
  end
end

OutputWriters["html"] = HTMLWriter