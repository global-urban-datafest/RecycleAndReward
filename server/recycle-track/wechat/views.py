from django.shortcuts import render
from django.conf import settings
from django.http import HttpResponse
import hashlib, logging

log = logging.getLogger(__name__)

# Create your views here.
def handle_wechat_auth(request):
    """
    Private method for handling wechat authentication before using the account
    """
    signature = request.REQUEST.get('signature', None)
    timestamp = request.REQUEST.get('timestamp', None)
    nonce = request.REQUEST.get('nonce', None)
    echostr = request.REQUEST.get('echostr', None)

    if signature and timestamp and nonce and echostr:
        token = settings.WECHAT_TOKEN
        l = [token, timestamp, nonce]
        l.sort()
        key_gen = hashlib.sha1(''.join(l)).hexdigest()

        if key_gen == signature: 
            log.info('Wechat authentication passed: key_gen({0}) signature:({1})'.format(key_gen, signature))
            return True, HttpResponse(echostr)
        else:
            log.info('Wechat authentication failed: key_gen({0}) signature:({1})'.format(key_gen, signature))
            log.info("request: {0} sorted str: {1}".format(request.REQUEST, "".join(l)))
            return True, HttpResponse('Unauthorized wechat request.')

    return False, None


def wechat_entry(request):
    
    # handle auth request
    is_auth, response = handle_wechat_auth(request)
    if is_auth:
        return response


    # handle other requests
    

    return HttpResponse('OK')
