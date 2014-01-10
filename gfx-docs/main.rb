require 'json'
require 'optparse'

require_relative 'parser'
require_relative 'scanner'

require_relative 'writers/jsonwriter'
require_relative 'writers/htmlwriter'

if __FILE__ == $0
  options = {
    verbose: false, 
    output_type: "json",
    output_destination: $stdout,
  }
  OptionParser.new do |opts|
    opts.banner = "Usage: #{File.basename(__FILE__)} [options]"
    
    opts.on("-v", "--[no-]verbose", "Run verbosely") do |verbose|
      options[:verbose] = verbose
    end
    
    opts.on("-t", "--output-type [TYPE]", "The type of output to include") do |output_type|
      options[:output_type] = output_type
    end
    
    opts.on("-o", "--output-destination [DESTINATION]", "Where to put the output") do |output_destination|
      options[:output_destination] = output_destination
    end
  end.parse!
  
  output_writer = OutputWriters[options[:output_type]]
  abort "Unknown output type '#{options[:output_type]}'." unless output_writer
  
  modules = []
  ARGV.each do |path|
    filename = File.basename(path)
    puts "Reading #{filename}" if options[:verbose]
    contents = File.read(path)
    
    puts "Parsing #{filename}" if options[:verbose]
    parsed_contents = Parser.new(contents).go
    
    puts "Analyzing #{filename}" if options[:verbose]
    result = Scanner.new(parsed_contents, filename: filename).go
    
    modules << result.module
  end
  
  output_writer.generate(modules, options[:output_destination])
  
  puts "" if options[:output_destination] == $stdout
end