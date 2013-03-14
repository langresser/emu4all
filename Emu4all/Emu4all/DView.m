#import "DView.h"
#import "iosUtil.h"
#import <QuartzCore/QuartzCore.h>  

CGRect drects[100];
int ndrects;

@implementation DView


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        // Initialization code
		self.backgroundColor = [UIColor clearColor];
		self.multipleTouchEnabled = NO;
	    self.userInteractionEnabled = NO;
	
	   
    }
    return self;
}


- (void)drawRect:(CGRect)rect {
	CGContextRef context = UIGraphicsGetCurrentContext();
	 
   	//Set the stroke (pen) color
	CGContextSetStrokeColorWithColor(context, [UIColor redColor].CGColor);
	//Set the width of the pen mark
	CGContextSetLineWidth(context, 2.0);
	
    int i=0;
    
	for(i=0; i<ndrects;i++)
	  CGContextStrokeRect(context, drects[i]);
	
	//CGContextAddRect(context, drects[1]);
	//Draw it
	CGContextFillPath(context);
	       
    CGContextSelectFont(context, "Helvetica", 16, kCGEncodingMacRoman); 
    CGContextSetTextDrawingMode (context, kCGTextFillStroke);
    CGContextSetRGBFillColor (context, 0, 5, 0, .5);
    CGRect viewBounds = self.bounds;
    CGContextTranslateCTM(context, 0, viewBounds.size.height);
    CGContextScaleCTM(context, 1, -1);
    CGContextSetRGBStrokeColor (context, 0, 1, 1, 1);
 
   if(!isPad())
   {
 //     CGContextShowTextAtPoint(context, 10, 10, "Es un iPhone",12 );
   }
   else
   {
      //CGContextShowTextAtPoint(context, 10, 10, "Es un iPad",10 );
   }
		
}

@end
