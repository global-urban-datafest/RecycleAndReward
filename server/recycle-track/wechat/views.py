from django.shortcuts import render
from django.conf import settings
from django.http import HttpResponse

# Create your views here.

def wechat_entry(request):
    token = settings.WECHAT_TOKEN
    print(request.REQUEST)

    return HttpResponse('OK')
