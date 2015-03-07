from django.shortcuts import render
from django.http import HttpResponse
from traceapp.models import *

# Create your views here.

class ReturnCode(object):
    """
    List of Return Codes
    """
    ACCEPTED = '00'
    BARCODE_USED = '01'
    BARCODE_INVALID = '02'
    ACTION_FAILED = '03'
    FIELDS_NOT_ENOUGH = '04'
    CHECKPOINT_INVALID = '05'
    WEIGHT_INVALID = '06'

def garbage_upload(request):
    """
    HTTP GET: /traceapp/upload/
    Parameters:
        #userid => Integer, user's id
        barcode => String, garbage bag's barcode
        weight => Float, garbage bag's weight
        #checkpoint => Integer, check point's id
        key => String, check point's key

    """
    barcode = request.REQUEST.get('barcode', None)
    weight = request.REQUEST.get('weight', None)
    key = request.REQUEST.get('key', None)

    if barcode and weight and key:
        # proceed dump action

        # validate bag status
        try:
            bag = Bag.objects.get(barcode=barcode)
            if bag.status > 1 : # BAG_STATUS: 1 = Assigned
                return HttpResponse(ReturnCode.BARCODE_USED)
            elif bag.status != 1 or not bag.user:
                return HttpResponse(ReturnCode.BARCODE_INVALID)
        except Bag.DoesNotExist:
            return HttpResponse(ReturnCode.BARCODE_INVALID)

        # validate check point
        try:
            checkpoint = CheckPoint.objects.get(key=key)
        except CheckPoint.DoesNotExist:
            return HttpResponse(ReturnCode.CHECKPOINT_INVALID)

        trace = BagTrace.objects.create(user=bag.user, bag=bag, check_point=checkpoint, event=1, extra=weight) # dump bag
        trace.save()

        # validate weight
        try:
            if not weight or float(weight) <= 0 or float(weight) > GarbageConstants.BAG_MAX_WEIGHT:
                trace = BagTrace.objects.create(user=bag.user, bag=bag, check_point=checkpoint, event=3, extra=weight) # reject weight
                trace.save()
                return HttpResponse(ReturnCode.WEIGHT_INVALID)
        except:
            trace = BagTrace.objects.create(user=bag.user, bag=bag, check_point=checkpoint, event=1, extra=weight) # reject weight
            trace.save()
            return HttpResponse(ReturnCode.WEIGHT_INVALID)

        # calculate weight and bonus
        bag_weight = float(weight)
        bag.weight = bag_weight
        bag.status = 2
        bag.save()

        profile = bag.user.profile
        profile.points_onway += bag.weight * 100
        profile.total_weight += bag.weight
        profile.save()        

        # insert bag trace record
        trace = BagTrace.objects.create(user=bag.user, bag=bag, check_point=checkpoint, event=2, extra=weight) # accept bag
        trace.save()
    
        # succeeded
        return HttpResponse(ReturnCode.ACCEPTED)

    else:
        return HttpResponse(ReturnCode.FIELDS_NOT_ENOUGH)



def garbage_checkpoint(request):
    """
    HTTP GET: /traceapp/checkpoint/
    Parameters:
        barcode => String, garbage bag's barcode
        #checkpoint => Integer, check point's id
        key => String, check point's key
        action => Integer, check point's action

    """

    barcode = request.REQUEST.get('barcode', None)
    key = request.REQUEST.get('key', None)
    action = request.REQUEST.get('action', '4')

    if barcode and key and action:
        # proceed transfer action

        # validate bag status
        try:
            bag = Bag.objects.get(barcode=barcode)
            if bag.status < 1 : # BAG_STATUS: 1 = Assigned, 2 = Used
                return HttpResponse(ReturnCode.BARCODE_INVALID)
        except DoesNotExist:
            return HttpResponse(ReturnCode.BARCODE_INVALID)

        # validate check point
        try:
            checkpoint = CheckPoint.objects.get(key=key)
        except DoesNotExist:
            return HttpResponse(ReturnCode.CHECKPOINT_INVALID)

        # validate action
        try:
            checkpoint_action = int(action)

            # insert check point trace record
            if checkpoint_action == 4: # bag arrived
                bag.status = 3 # Bag: Transfer               
                bag.save()
    
                trace = BagTrace.objects.create(user=bag.user, bag=bag, check_point=checkpoint, event=checkpoint_action)
                trace.save()
                
                # action succeeded
                return HttpResponse(ReturnCode.ACCEPTED)
        except:
            pass

        # action invalid
        return HttpResponse(ReturnCode.ACTION_FAILED)

    else:
        return HttpResponse(ReturnCode.FIELDS_NOT_ENOUGH)

    return HttpResponse('Checkpoint OK')


def garbage_validate(request):
    """
    HTTP GET: /traceapp/validate/
    Parameters:
        checkpoint => Integer, check point's id
        barcode => String, garbage bag's barcode
        action => Integer, check point's action
        reason => Integer, check point's action reason

    """
    barcode = request.REQUEST.get('barcode', None)
    key = request.REQUEST.get('key', None)
    action = request.REQUEST.get('action', None)

    if barcode and key and action:
        # proceed transfer action

        # validate bag status
        try:
            bag = Bag.objects.get(barcode=barcode)
            if bag.status < 1 : # BAG_STATUS: 1 = Assigned, 2 = Used
                return HttpResponse(ReturnCode.BARCODE_INVALID)
        except DoesNotExist:
            return HttpResponse(ReturnCode.BARCODE_INVALID)

        # validate check point
        try:
            checkpoint = CheckPoint.objects.get(key=key)
        except DoesNotExist:
            return HttpResponse(ReturnCode.CHECKPOINT_INVALID)

        # validate action
        try:
            checkpoint_action = int(action)

            # insert check point trace record
            if checkpoint_action == 4 and bag.status <= 4: # bag arrived
                bag.status = 3 # Bag: Transfer               
                bag.save()

                trace = BagTrace.objects.create(user=bag.user, bag=bag, check_point=checkpoint, event=checkpoint_action)
                trace.save()

                # action succeeded
                return HttpResponse(ReturnCode.ACCEPTED)

            elif checkpoint_action == 5 and bag.status < 4 and bag.status >= 2: # bag passed
                bag.status = 4
                bag.save()

                # update user's points
                profile = bag.user.profile
                profile.points += bag.weight * 100
                profile.points_onway -= bag.weight
                profile.save()
                
                # update trace
                trace = BagTrace.objects.create(user=bag.user, bag=bag, check_point=checkpoint, event=checkpoint_action)
                trace.save()
                
                return HttpResponse(ReturnCode.ACCEPTED)

            elif checkpoint_action == 6 and bag.status < 4 and bag.status >= 2: # bag failed
                bag.status = 4
                bag.save()

                # update user's points
                profile = bag.user.profile
                profile.points_failed -= bag.weight * 100
                profile.points_onway -= bag.weight
                profile.save()

                # update trace
                trace = BagTrace.objects.create(user=bag.user, bag=bag, check_point=checkpoint, event=checkpoint_action)
                trace.save()

                return HttpResponse(ReturnCode.ACCEPTED)
            
        except:
            pass

        return HttpResponse(ReturnCode.ACTION_FAILED)
    
    else:
        return HttpResponse(ReturnCode.FIELDS_NOT_ENOUGH)

