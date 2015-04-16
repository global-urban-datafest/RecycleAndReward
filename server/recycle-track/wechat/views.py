from django.shortcuts import render
from django.conf import settings
from django.http import HttpResponse
from django.views.decorators.csrf import csrf_exempt
import hashlib, logging, time
from lxml import etree
from common.logging_utils import logging_response

log = logging.getLogger(__name__)

# Create your views here.

def authenticate_wechat(request):
    signature = request.REQUEST.get('signature', None)
    timestamp = request.REQUEST.get('timestamp', None)
    nonce = request.REQUEST.get('nonce', None)

    if signature and timestamp and nonce:
        token = settings.WECHAT_TOKEN
        l = [token, timestamp, nonce]
        l.sort()
        key_gen = hashlib.sha1(''.join(l)).hexdigest()

        if key_gen == signature:
            log.debug('Wechat authentication passed: key_gen({0}) signature:({1})'.format(key_gen, signature))
            return True
        else:
            log.debug('Wechat authentication failed: key_gen({0}) signature:({1})'.format(key_gen, signature))
            log.debug("request: {0} sorted str: {1}".format(request.REQUEST, "".join(l)))
            return False


def render_base_message(request, to_user, message):
    return render(request, 'base_message.tmpl', {
            'from_user': settings.WECHAT_ACCOUNT,
            'to_user': to_user,
            'message': message,
            'timestamp': int(time.time()),
        })

@csrf_exempt
@logging_response
def wechat_entry(request):
    
    # handle auth request
    if not authenticate_wechat(request):
        return HttpResponse('Wechat authenticate failed.')


    # if it's account authenticate request
    echostr = request.REQUEST.get('echostr', None)
    if echostr: 
        return HttpResponse(echostr)


    # handle other requests
    tree = etree.fromstring(request.body)
    if tree.xpath('/xml/FromUserName/text()'):
        to_user = tree.xpath('/xml/FromUserName/text()')[0]
    else:
        return HttpResponse('Not supported')

    if tree.xpath('/xml/Event/text()') == ['subscribe']:
        return render_base_message(request, to_user, 'Welcome to join Recycle and Reward Project!')

    return render_base_message(request, to_user, "We're still developing new functions. Let's keep in touch!")

