import logging

httplog = logging.getLogger('common.logging.http')

def logging_response(func):
    """
    A simple logging decorator for debugging response data
    """
    def func_wrapper(request):
        httplog.debug('request: {0}?{1}'.format(request.path, request.META['QUERY_STRING'])) # print request
        if request.body:
            httplog.debug('post: {0}'.format(request.body))
        response = func(request)
        httplog.debug('response: {0}'.format(response.content)) # print response
        return response
    return func_wrapper
