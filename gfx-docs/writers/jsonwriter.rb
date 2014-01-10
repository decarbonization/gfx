require 'json'
require_relative 'outputwriter'

class JSONWriter < OutputWriter
  def self.generate(modules, destination)
    result = JSON.generate(modules.map(&:to_hash))
    if destination.is_a? String
      File.open(destination, "w") do |f|
        f.write(result)
      end
    else
      destination.write(result)
    end
  end
end

OutputWriters["json"] = JSONWriter