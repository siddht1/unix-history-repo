#include <X/mit-copyright.h>

/* $Header: XPeekEvent.c,v 10.4 86/02/01 15:37:48 tony Rel $ */
/* Copyright    Massachusetts Institute of Technology    1985	*/

#include "XlibInternal.h"

extern _QEvent *_qfree;

/* Flush output and (wait for and) return the next event in the queue,
*  BUT do not remove it from the queue.
*/

XPeekEvent (event)
	register XEvent *event;
{
	register _QEvent *qelt;
	register Display *dpy = _XlibCurrentDisplay;
	
	_XFlush (dpy);
	if (qelt = dpy->head) {
	    *event = qelt->event;
	    return;
	    }

	while (1) {
	    _XRead (dpy, event, sizeof(XEvent));
	    if (event->type == X_Error)
	    	_XError (dpy, (XErrorEvent *) event);
	    else {  /* it's an event packet */
	    	_XEnq (dpy, event);
		return;
		}
	}
}

