import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

/**
 * Tests out a compression program on a number of input files.
 * The program should be an executable file that takes two command line arguments:
 * the input file and the output file. Tester will run it with every file in the input
 * DIRECTORY you specify, and have it spit them out in the output directory you specify.
 * The program must not be a directory, but rather a single file.
 * 
 * Currently supported filetypes:
 * 	-.class (Java)
 * 
 * Note that Tester does NOT currently iterate subdirectories. Be sure to put all your
 * input files in the same directory.
 * 
 * For each file, Tester will tell you the original size, compressed size, and compression
 * ratio. At the end, it will tell you the average compression ratio.
 * 
 * Currently, Tester does not do validation of any sort! It's up to you to verify that your
 * program does indeed spit out decompressible files.
 * 
 * Usage for Tester is as follows:
 * 
 * java Tester <program> <input directory> <output directory>
 * 
 * @author Ryan Beck
 */
public class Tester {
	
	static String progPath, inputPath, outputPath;
	
	public static void main(String[] args) throws IOException, InterruptedException {
		
		if(args.length != 3){
			printUsage();
			System.exit(10022);
		}
		
		progPath = args[0];
		inputPath = args[1];
		outputPath = args[2];
		
		File program = new File(progPath);
		if(!program.exists()){
			throw new FileNotFoundException("Program file \""+progPath+"\"\ndoes not exist.");
		}
		if(!program.canExecute()){
			throw new IOException("Program file \""+progPath+"\"\ncannot be executed.");
		}
		
		String baseCmd = "";
		
		if(getExtension(program).equals("class")){
			
			String name = program.getName();
			name = name.substring(0,name.length()-6);
			
			baseCmd = "java -cp \""+program.getParentFile()+"\" "+name;
			System.out.println("Running program with: "+baseCmd+" <input file> <output file>\n");
			
		}else if(getExtension(program).equals("bat")){
			
			baseCmd = "\""+program.getAbsolutePath()+"\"";
			System.out.println("Running program with: "+baseCmd+" <input file> <output file>\n");
			
		}else{
			throw new IllegalArgumentException(getExtension(program)+" files are currently unsupported.");
		}
		
		File input = new File(inputPath), output = new File(outputPath);

		if(!input.exists()){
			throw new FileNotFoundException("Input directory does not exist");
		}
		if(!input.isDirectory()){
			throw new IllegalArgumentException("Input must be directory");
		}
		
		if(!output.exists() && !output.mkdir()){
			throw new IOException("Could not create output directory");
		}
		if(!output.isDirectory()){
			throw new IllegalArgumentException("Output must be directory");
		}
		
		double avg = 0.0;
		int successCount = 0;
		
		for(File f : input.listFiles()){
			
			System.out.println("Compressing "+f.getName()+"...");
			File out = new File(outputPath+File.separator+f.getName()+".out");
			
			long originalSize = f.length(), compressedSize;
			System.out.printf("Original size: %s\n",humanReadableByteCount(originalSize,false));
			
			String cmd = baseCmd+" \""+f.getPath()+"\" \""+out.getPath()+"\"";
			//System.out.println("cmd: "+cmd);
			
			Process p = Runtime.getRuntime().exec(cmd);
			p.waitFor(); //This is a top-level concurrent programming technique. Don't even ATTEMPT to understand it.
			
			if(p.exitValue()!=0){
				System.out.println("...but something went wrong!");
				System.out.println();
				continue;
			}
			
			if(out.exists()){
				successCount++;
				compressedSize = out.length();
				double ratio = (double)compressedSize/originalSize;
				avg += ratio;
				System.out.println("Compressed size: "+humanReadableByteCount(compressedSize,false));
				System.out.printf("Compression ratio: %.3f\n",ratio);
			}else{
				System.out.println("Program did not output the expected file!");
			}
			
			System.out.println();
			
		}
		
		System.out.println("Done.\n");
		System.out.printf("Average compression ratio: %.3f\n",avg/successCount);
			
	}
	
	public static String getExtension(File file){
		int x = file.getPath().lastIndexOf('.');
		if(x==-1)
			return "";
		return file.getPath().substring(x+1);
	}
	
	/**
	 * A nifty little to string method for displaying file sizes. It's entirely self-contained!
	 * Thanks to StackOverflow user aioobe.
	 * https://stackoverflow.com/questions/3758606/how-to-convert-byte-size-into-human-readable-format-in-java
	 * @param bytes the number of bytes to display
	 * @param si whether to divide by multiples of 1,000 (true), or multiples of 1,024 (false)
	 * @return a human-readable string representing the given number of bytes.
	 */
	public static String humanReadableByteCount(long bytes, boolean si) {
	    int unit = si ? 1000 : 1024;
	    if (bytes < unit) return bytes + " B";
	    int exp = (int) (Math.log(bytes) / Math.log(unit));
	    String pre = (si ? "kMGTPE" : "KMGTPE").charAt(exp-1) + "";
	    return String.format("%.1f %sB", bytes / Math.pow(unit, exp), pre);
	}
	
	public static void printUsage(){
		
		System.out.println("Usage is <program> <input directory> <output directory>");
		
	}
	
}


