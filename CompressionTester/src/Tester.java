import java.io.BufferedReader;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.util.Scanner;

/**
 * Tests out a list of compression programs on a file corpus.
 * Each program to be run is specified as a single-line command in a text file.
 * The strings <input> and <output> will be replaced with a file from the corpus, 
 * and a temp file for the output.
 * Tester will run each program with every file in the input DIRECTORY you specify (the corpus),
 * and have it spit them out in the output directory you specify.
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
 * java Tester <program list> <input directory> <output directory>
 * 
 * @author Ryan Beck
 */
public class Tester {
	
	static String progPath, inputPath, outputPath, progName;
	static FileWriter csvWriter;
	
	public static void main(String[] args) throws IOException, InterruptedException {
		
		if(args.length != 3){
			printUsage();
			System.exit(10022);
		}
		
		progPath = args[0];
		inputPath = args[1];
		outputPath = args[2];
		
		File programList = new File(progPath), input = new File(inputPath), output = new File(outputPath);

		if(!programList.exists()){
			exit("Program file \""+progPath+"\"\ndoes not exist.");
		}
		if(!programList.canRead()){
			exit("Cannot read program file \""+progPath+"\".");
		}
		if(!input.exists()){
			exit("Input directory does not exist");
		}
		if(!input.isDirectory()){
			exit("Input must be directory");
		}
		if(!output.exists() && !output.mkdir()){
			exit("Could not create output directory");
		}
		if(!output.isDirectory()){
			exit("Output must be directory");
		}
		
		try{
			openCSV("results.csv");
		}catch(IOException e){
			exit("Could not create results file "+e.getMessage());
		}
		
		Scanner in = new Scanner(programList);
		int line;
		
		for(line=1; in.hasNextLine(); line++){
			
			String baseCmd = parse(in.nextLine());
			
			if(baseCmd.length()==0) //line is blank or a comment
				continue;
			
			if(progName==null) //global varible set by the parse function
				progName = baseCmd;
			
			if(!baseCmd.contains("<input>") || !baseCmd.contains("<output>")){
				System.out.println("Malformed command on line "+line+"!");
				progName = null;
				continue;
			}
			
			System.out.printf("Running line %d:\n%s\n%s\n\n",line,baseCmd,progName==null ? "(No name)" : ("Name: "+progName));
			
			double avg = 0.0, avgTime = 0.0;
			int successCount = 0;
			for(File f : input.listFiles()){
				
				if(f.isDirectory()){ //Tester does not recurse as of right now
					//System.out.printf("Encountered directory %s.\n\n",f.getName());
					continue;
				}
				
				System.out.println("Compressing "+f.getName()+"...");
				File out = new File(outputPath+File.separator+f.getName()+".out");
				if(out.exists()){
					try{
						Files.delete(out.toPath()); //Files.delete throws an exception when it can't be deleted! I want that.
					}catch(IOException e){
						exit("Could not delete existing output file: "+e.getMessage());
					}
				}
				
				long originalSize = f.length(), compressedSize;
				//System.out.printf("Original size: %s\n",humanReadableByteCount(originalSize,false));
				
				//String cmd = baseCmd+" \""+f.getPath()+"\" \""+out.getPath()+"\"";
				String cmd = baseCmd.replace("<input>", '"'+f.getPath()+'"').replace(
						"<output>", '"'+out.getPath()+'"');
				//System.out.println("cmd: "+cmd);
				
				Process p = Runtime.getRuntime().exec(cmd);
				long time = -System.currentTimeMillis();
				
				ProcessKiller hook = new ProcessKiller(p);  //Register a hook to shut p down when the main thread exits.
				Runtime.getRuntime().addShutdownHook(hook); //It's super unreliable!
				gobble(p.getInputStream()); //might need to gobble p.getErrorStream() as well.
				
				p.waitFor(); //This is a top-level concurrent programming technique. Don't even ATTEMPT to understand it.
				time += System.currentTimeMillis();
				
				Runtime.getRuntime().removeShutdownHook(hook);
				
				if(p.exitValue()!=0){
					System.out.printf("Runtime error (%d) encountered when compressing %s!\n",p.exitValue(),f.getName());
					System.out.println("The error stream had this to say:");
					Scanner err = new Scanner(p.getErrorStream());
					while(err.hasNextLine())
						System.out.println(err.nextLine());
					System.out.println();
					err.close();
					continue;
				}
				
				if(out.exists()){
					successCount++;
					compressedSize = out.length();
					//double ratio = (double)compressedSize/originalSize;
					double ratio = (double)originalSize/compressedSize;
					avg += ratio;
					avgTime += time;
					
					//OUTPUT DATA TO CSV HERE
					writeToCSV(progName, f.getName(), originalSize, compressedSize, time);
					
					//System.out.println("Compressed size: "+humanReadableByteCount(compressedSize,false));
					//System.out.println("("+compressedSize+")");
					System.out.println("Time: "+humanReadableDuration(time));
					System.out.printf("Ratio: %.3f\n",ratio);
				}else{
					System.out.println("Program did not output the expected file when compressing "+f.getName()+"!");
				}
				
			}
			
			System.out.println("Done.\n");
			System.out.printf("Average smallness increasement factor: %.3f\n\n\n", avg/successCount);
			writeToCSV(progName, avg/successCount, avgTime/successCount);
			progName = null;
			
		}
		
		System.out.println("All done!");
		in.close();
		closeCSV();
			
	}
	
	//om nom nom
	public static void gobble(InputStream input) throws IOException{
		BufferedReader in = new BufferedReader(new InputStreamReader(input));
		while(in.readLine()!=null);
		//for(String line;(line=in.readLine())!=null;)
		//	System.out.println("STDOUT > "+line);
	}
	
	public static String clamp(String str){
		int start = 0, end = str.length()-1;
		for(;start<str.length() && str.charAt(start)==' ';start++);
		for(;end>=0 && str.charAt(end)==' ';end--);
		return end<0 ? "" : str.substring(start,end+1);
	}
	
	public static String parse(String str){
		int octoIndex = str.indexOf('#'); //Index of octopus. I mean, octothorpe.
		String cmd = str, comment = "";
		if(octoIndex>=0){
			cmd = str.substring(0,octoIndex);
			comment = str.substring(octoIndex+1,str.length());
		}
		
		cmd = clamp(cmd);
		comment = clamp(comment);
		int nameIndex = comment.indexOf(":name ");
		if(nameIndex>=0){
			progName = comment.substring(nameIndex+6, comment.length());
		}
		
		return cmd;
		
	}
	
	public static String getExtension(File file){
		int x = file.getPath().lastIndexOf('.');
		if(x==-1)
			return "";
		return file.getPath().substring(x+1);
	}
	
	/**
	 * A nifty little tostring method for displaying file sizes. It's entirely self-contained!
	 * Thanks to StackOverflow user aioobe. <br/>
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
	
	public static String humanReadableDuration(long t){
		if(t<1000)
			return t+" ms";
		if(t<60000)
			return String.format("%d.%03d s" , t/1000, t%1000);
		return String.format("%d m %d s", t/60000, (t%60000)/1000);
	}
	
	public static void printUsage(){	
		System.out.println("Usage is:\njava Tester <program list> <input directory> <output directory>");
	}
	
	private static void openCSV(String path) throws IOException{
		csvWriter = new FileWriter(path,false); //DON'T append to end of file
		csvWriter.write("Program, File, Original Size, Compressed Size, Compression Ratio, Time, Nerdy Time\n"); //header
	}
	
	private static void closeCSV() throws IOException{
		csvWriter.close();
		csvWriter = null;
	}
	
	public static void writeToCSV(String program, double avg, double avgTime) throws IOException{
		if(csvWriter==null)
			openCSV("results.csv");
		csvWriter.write(program+",Average, , ,"+avg+","+humanReadableDuration((long)avgTime)+","+avgTime+"\n");
	}
	
	public static void writeToCSV(String program, String fileName, long originalSize, long compressedSize, long time) throws IOException{
		if(csvWriter==null)
			openCSV("results.csv");
		csvWriter.write(program+","+fileName+","+humanReadableByteCount(originalSize,false)+","+humanReadableByteCount(compressedSize,false)+","+((double)originalSize/compressedSize)+","+
				humanReadableDuration(time)+","+time+"\n");
	}
	
	static void exit(String msg){
		System.err.println(msg);
		System.exit(1);
	}
	
	static class ProcessKiller extends Thread{
		Process p;
		public ProcessKiller(Process p){
			super();
			this.p = p;
		}
		public void run(){
			System.out.println("Terminating subprocess...");
			p.destroyForcibly();
		}
	}
	
}

