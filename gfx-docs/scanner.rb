require_relative 'model/directive'
require_relative 'model/docinfo'

class ScannerResult
  attr_accessor :filename
  attr_accessor :module
  attr_accessor :errors
  
  def initialize(filename: nil)
    @filename = filename
    @module = ModuleDocInfo.new
    @module.name = filename || "anonymous"
    @errors = []
  end
end

class Scanner
  attr_reader :directive_chunks
  attr_reader :filename
  
  def initialize(directive_chunks, filename: nil)
    @directive_chunks = directive_chunks
    @filename = filename
  end
  
  def go
    result = ScannerResult.new(filename: self.filename)
    
    @directive_chunks.each do |directives|
      current_doc_info = nil
      directives.each do |directive|
        unless current_doc_info
          if DocInfoDirectiveNames.include? directive.name then
            current_doc_info = DocInfoClasses[directive.name].new
            current_doc_info.name = directive.contents
            
            if current_doc_info.is_a? ModuleDocInfo
              result.module = current_doc_info
            end
          else
            result.errors << "*** Warning, unsupported doc type #{directive.name}"
          end 
          
          next
        end
        
        directive_sel = (directive.name + "=").to_sym
        if current_doc_info.respond_to? directive_sel then
          current_doc_info.send(directive_sel, directive.contents)
        else
          result.errors << "*** Warning, did not recognize directive #{directive.name}, ignoring."
        end
      end
      
      if current_doc_info && !current_doc_info.is_a?(ModuleDocInfo)
        result.module.doc_infos << current_doc_info
      end
    end
    
    result
  end
end