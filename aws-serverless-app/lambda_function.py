import os
import json
import boto3


arn = os.environ.get('SNS_TOPIC')

def lambda_handler(event, context):
    print(event)
    message = event
    client = boto3.client('sns')
    response = client.publish(
        TargetArn=arn,
        Message=json.dumps({'default': json.dumps(message)}),
        MessageStructure='json'
    )
    return message